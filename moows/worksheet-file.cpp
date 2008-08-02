#include "moows/worksheet-p.h"
#include "moows/worksheet-xml.h"

using namespace moo::ws;

#define ELM_WORKSHEET       "ggap-worksheet"
#define ELM_CONTENT         "content"
#define ELM_INPUT           "input"
#define ELM_OUTPUT          "output"
#define ELM_TEXT            "text"
#define ELM_HISTORY         "history"
#define ELM_ITEM            "item"

#define PROP_VERSION        "version"
#define WS_MAJOR_VERSION    2
#define WS_MINOR_VERSION    1
#define WS_VERSION          "2.1"

#define PROP_ID             "id"

#define PROP_STDOUT         "stdout"
#define PROP_STDERR         "stderr"
#define PROP_OUTPUT_TYPE    "type"
#define PROP_OUTPUT_STDOUT  "out"
#define PROP_OUTPUT_STDERR  "err"


static void check_id(const QDomElement &elm, Block *b, QMap<QString, Block*> &id_map)
{
    QString id = elm.attribute(PROP_ID);
    if (!id.isEmpty())
        id_map[id] = b;
}

void WorksheetPrivate::load_block_content(const QDomElement &elm, TextBlockBase *block)
{
    const rt::Fragment fragment = rt::Fragment::fromWsXml(elm);
    QTextCursor cr = block->startCursor();
    doc.insertFragment(cr, fragment);
}

void WorksheetPrivate::load_input(const QDomElement &elm,
                                  QMap<QString, Block*> &id_map,
                                  QMap<PromptBlock*, QStringList> &output_map)
{
    PromptBlock *b = new PromptBlock;
    doc.appendBlock(b);
    load_block_content(elm, b);
    check_id(elm, b, id_map);
    QString out = elm.attribute(PROP_STDOUT);
    QString err = elm.attribute(PROP_STDERR);
    if (!out.isEmpty() || !err.isEmpty())
        output_map[b] << out << err;
}

void WorksheetPrivate::load_output(const QDomElement &elm, QMap<QString, Block*> &id_map)
{
    QString out_type = elm.attribute(PROP_OUTPUT_TYPE);
    if (out_type.isEmpty())
    {
        qCritical("%s: %s property missing", Q_FUNC_INFO, PROP_OUTPUT_TYPE);
        out_type = PROP_OUTPUT_STDOUT;
    }

    bool is_stdout = out_type == PROP_OUTPUT_STDOUT;
    OutputBlock *b = new OutputBlock(is_stdout);
    doc.appendBlock(b);
    load_block_content(elm, b);
    check_id(elm, b, id_map);
}

void WorksheetPrivate::load_text(const QDomElement &elm, QMap<QString, Block*> &id_map)
{
    TextBlock *b = new TextBlock;
    doc.appendBlock(b);
    load_block_content(elm, b);
    check_id(elm, b, id_map);
}

static bool parse_version(const QString &version, uint *pmajor, uint *pminor)
{
    QStringList parts = version.split('.');
    if (parts.size() != 2)
        return false;

    bool ok, ok2;
    long major = parts.at(0).toLong(&ok);
    long minor = parts.at(1).toLong(&ok2);
    if (!ok || !ok2)
        return false;

    if (major < 1 || major > 1000000 ||
        minor < 0 || minor > 1000000)
            return false;

    *pmajor = major;
    *pminor = minor;
    return true;
}

static bool check_version(const QString &version, QString &error)
{
    uint major = 0, minor = 0;

    if (!parse_version(version, &major, &minor))
    {
        error = QString("bad version '%1'").arg(version);
        return false;
    }

    if (major != WS_MAJOR_VERSION)
    {
        error = QString("incompatible version '%1'").arg(version);
        return false;
    }

    if (minor > WS_MINOR_VERSION)
    {
        error = "file was saved with newer version of ggap";
        return false;
    }

    return true;
}

void WorksheetPrivate::load_content(const QDomElement &content_elm)
{
    QMap<QString, Block*> id_map;
    QMap<PromptBlock*, QStringList> output_map;

    QDomNodeList children = content_elm.childNodes();
    for (int i = 0; i < children.size(); ++i)
    {
        QDomNode child = children.at(i);

        if (!child.isElement())
            continue;

        QDomElement elm = child.toElement();
        if (elm.isNull())
            continue;

        if (elm.tagName() == ELM_INPUT)
            load_input(elm, id_map, output_map);
        else if (elm.tagName() == ELM_OUTPUT)
            load_output(elm, id_map);
        else if (elm.tagName() == ELM_TEXT)
            load_text(elm, id_map);
        else
            qCritical() << QString("%1: unknown element %2").arg(Q_FUNC_INFO).arg(elm.tagName());
    }

    QMapIterator<PromptBlock*, QStringList> iter(output_map);
    while (iter.hasNext())
    {
        iter.next();
        PromptBlock *pb = iter.key();
        QStringList out_ids = iter.value();
        for (int i = 0; i < 2; ++i)
        {
            if (!out_ids[i].isEmpty())
            {
                QMap<QString, Block*>::const_iterator j = id_map.constFind(out_ids[i]);
                if (j != id_map.constEnd() && isOutputBlock(*j))
                    pb->setOutputBlock(i, static_cast<OutputBlock*>(*j));
            }
        }
    }
}

void WorksheetPrivate::load_history(const QDomElement &history_elm)
{
    QStringList items;

    QDomNodeList children = history_elm.childNodes();
    for (int i = 0; i < children.size(); ++i)
    {
        QDomNode child = children.at(i);

        if (!child.isElement())
            continue;

        QDomElement elm = child.toElement();
        if (elm.isNull())
            continue;

        if (elm.tagName() == ELM_ITEM)
            items << elm.text();
        else
            qCritical() << QString("%1: unknown element %2").arg(Q_FUNC_INFO).arg(elm.tagName());
    }

    setHistory(items);
}

bool Worksheet::loadXml(const QString &filename, QString &error)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        error = file.errorString();
        return false;
    }
    QDomDocument xml;
    if (!xml.setContent(&file, &error))
        return false;
    file.close();
    return loadXml(xml, error);
}

bool Worksheet::loadXml(const QDomDocument &xml, QString &error)
{
    QDomElement root = xml.documentElement();
    if (root.tagName() != ELM_WORKSHEET)
    {
        error = ELM_WORKSHEET " element missing";
        return false;
    }

    QString version = root.attribute(PROP_VERSION);
    if (version.isEmpty())
    {
        error = "document version attribute missing";
        return false;
    }

    if (!check_version(version, error))
        return false;

    reset();

    QDomNodeList children = root.childNodes();
    for (int i = 0; i < children.size(); ++i)
    {
        QDomNode child = children.at(i);

        if (!child.isElement())
            continue;

        QDomElement elm = child.toElement();
        if (elm.isNull())
            continue;

        if (elm.tagName() == ELM_CONTENT)
            impl->load_content(elm);
        else if (elm.tagName() == ELM_HISTORY)
            impl->load_history(elm);
        else
            qCritical() << QString("%1: unknown element %2").arg(Q_FUNC_INFO).arg(elm.tagName());
    }

    return true;
}


void Worksheet::format(QXmlStreamWriter &xml)
{
    xml.setAutoFormatting(true);
#if QT_VERSION >= 0x040400
    xml.setAutoFormattingIndent(1);
#endif
    xml.writeStartDocument();

    xml.writeStartElement(ELM_WORKSHEET);
    xml.writeAttribute(PROP_VERSION, WS_VERSION);

    QMap<Block*, QString> id_map;
    int count = 1;

    for (Block *block = firstBlock(); block != 0; block = block->next())
        id_map[block] = QString("%1").arg(count++);

    xml.writeStartElement(ELM_CONTENT);
    xml.writeAttribute("content-type", "richtext");

    for (Block *block = firstBlock(); block != 0; block = block->next())
    {
        if (PromptBlock *pb = dynamic_cast<PromptBlock*>(block))
        {
            xml.writeStartElement(ELM_INPUT);
            xml.writeAttribute(PROP_ID, id_map[pb]);

            OutputBlock *ob = pb->outputBlock(0);
            if (ob)
                xml.writeAttribute(PROP_STDOUT, id_map[ob]);
            ob = pb->outputBlock(1);
            if (ob)
                xml.writeAttribute(PROP_STDERR, id_map[ob]);

            formatBlock(xml, pb);
            xml.writeEndElement();
        }
        else if (OutputBlock *ob = dynamic_cast<OutputBlock*>(block))
        {
            xml.writeStartElement(ELM_OUTPUT);
            xml.writeAttribute(PROP_ID, id_map[ob]);
            if (ob->type() == OutputBlock::Stdout)
                xml.writeAttribute(PROP_OUTPUT_TYPE, PROP_OUTPUT_STDOUT);
            else
                xml.writeAttribute(PROP_OUTPUT_TYPE, PROP_OUTPUT_STDERR);
            formatBlock(xml, ob);
            xml.writeEndElement();
        }
        else if (TextBlock *tb = dynamic_cast<TextBlock*>(block))
        {
            xml.writeStartElement(ELM_TEXT);
            xml.writeAttribute(PROP_ID, id_map[tb]);
            formatBlock(xml, tb);
            xml.writeEndElement();
        }
        else
        {
            qCritical("%s: unknown block", Q_FUNC_INFO);
        }
    }

    xml.writeEndElement(); // content

    xml.writeStartElement(ELM_HISTORY);
    foreach (const QString &item, impl->history)
        xml.writeTextElement(ELM_ITEM, item);
    xml.writeEndElement(); // history

    xml.writeEndElement(); // worksheet
    xml.writeEndDocument();
}

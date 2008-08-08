#include "ggap/prefs.h"
#include "ggap/app.h"
#include "moo-macros.h"
#include <QtGui>

using namespace ggap;

QVariant Prefs::value(const QString &key, const QVariant &defaultValue) const
{
    return gapApp->settings(type)->value(key, defaultValue);
}

inline static bool equal_strings(const QString &s1, const QString &s2)
{
    if (s1.isEmpty())
        return s2.isEmpty();
    else if (s2.isEmpty())
        return false;
    else
        return s1 == s2;
}

inline static bool equal_fonts(const QString &fs1, const QString &fs2)
{
    QFont f1, f2;
    f1.fromString(fs1);
    f2.fromString(fs2);
    return f1 == f2;
}

inline static bool equal_values(const QVariant &v1, const QVariant &v2, bool *to_string = 0)
{
    if (to_string)
        *to_string = false;

    switch (v1.type())
    {
        case QVariant::String:
            return equal_strings(v1.toString(), v2.toString());

        case QVariant::ByteArray:
            return v1.toByteArray() == v2.toByteArray();

        case QVariant::Font:
            if (to_string)
                *to_string = true;
            return equal_fonts(v1.toString(), v2.toString());

        default:
            return v1 == v2;
    }
}

void Prefs::setValue(const QString &key, const QVariant &new_value, const QVariant &dflt)
{
    QVariant old_value = value(key, dflt);
    bool to_string = false;

    if (equal_values(new_value, old_value, &to_string))
        return;

    if (equal_values(new_value, dflt))
        gapApp->settings(type)->remove(key);
    else if (to_string)
        gapApp->settings(type)->setValue(key, new_value.toString());
    else
        gapApp->settings(type)->setValue(key, new_value);
}

void Prefs::unsetValue(const QString &key)
{
    gapApp->settings(type)->remove(key);
}

bool Prefs::value(BoolKey key)
{
    switch (key)
    {
        case SaveWorkspace:
            return value("gap/save-workspace", true).toBool();
        case UseDefaultGap:
            return value("gap/use-default", true).toBool();
    }

    m_return_val_if_reached(false);
}

void Prefs::setValue(BoolKey key, bool value)
{
    switch (key)
    {
        case SaveWorkspace:
            setValue("gap/save-workspace", value, true);
            return;
        case UseDefaultGap:
            setValue("gap/use-default", value, true);
            return;
    }

    m_return_if_reached();
}

QString Prefs::value(StringKey key)
{
    switch (key)
    {
        case GapExe:
            return value("gap/exe").toString();
        case GapRootDir:
            return value("gap/rootdir").toString();
        case GapArgs:
            return value("gap/args").toString();
        case HelpHomePage:
            return value("help/home").toString();
        case DefaultFileFormat:
            return value("gap-file/format", "workspace").toString();
    }

    m_return_val_if_reached(QString());
}

void Prefs::setValue(StringKey key, const QString &value)
{
    switch (key)
    {
        case GapExe:
            setValue("gap/exe", value);
            return;
        case GapRootDir:
            setValue("gap/rootdir", value);
            return;
        case GapArgs:
            setValue("gap/args", value);
            return;
        case HelpHomePage:
            setValue("help/home", value);
            return;
        case DefaultFileFormat:
            setValue("gap-file/format", value, "workspace");
            return;
    }

    m_return_if_reached();
}

QStringList Prefs::value(StringListKey key)
{
    switch (key)
    {
        case HelpBookmarks:
            return value("help/bookmarks").toStringList();
        case ExtraGapRoots:
            return value("gap/extraroots").toStringList();
    }

    m_return_val_if_reached(QStringList());
}

void Prefs::setValue(StringListKey key, const QStringList &value)
{
    switch (key)
    {
        case HelpBookmarks:
            setValue("help/bookmarks", value);
            return;
        case ExtraGapRoots:
            setValue("gap/extraroots", value);
            return;
    }

    m_return_if_reached();
}

QByteArray Prefs::value(ByteArrayKey key)
{
    switch (key)
    {
        case PageSetup:
            return value("print/page-setup").toByteArray();
    }

    m_return_val_if_reached(QByteArray());
}

void Prefs::setValue(ByteArrayKey key, const QByteArray &value)
{
    switch (key)
    {
        case PageSetup:
            setValue("print/page-setup", value);
            return;
    }

    m_return_if_reached();
}


#ifdef Q_OS_MAC
#define DEFAULT_WORKSHEET_FONT "Monaco, 11"
#elif defined(Q_OS_WIN32)
#define DEFAULT_WORKSHEET_FONT "Courier New"
#else
#define DEFAULT_WORKSHEET_FONT "Monospace"
#endif

QFont Prefs::value(FontKey key)
{
    QString fs;

    switch (key)
    {
        case WorksheetFont:
            fs = value("worksheet/font", DEFAULT_WORKSHEET_FONT).toString();
            break;
        case HelpFont:
            fs = value("help/font", QApplication::font()).toString();
            break;
    }

    m_return_val_if_fail(!fs.isEmpty(), QApplication::font());

    QFont font;
    font.fromString(fs);
    return font;
}

void Prefs::setValue(FontKey key, const QFont &value)
{
    switch (key)
    {
        case WorksheetFont:
            setValue("worksheet/font", value, DEFAULT_WORKSHEET_FONT);
            return;
        case HelpFont:
            setValue("help/font", value, QApplication::font());
            return;
    }

    m_return_if_reached();
}

int Prefs::value(IntKey key)
{
    switch (key)
    {
        case CompressionLevel:
            return value("gap-file/compression-level", -1).toInt();
    }

    m_return_val_if_reached(0);
}

void Prefs::setValue(IntKey key, int value)
{
    switch (key)
    {
        case CompressionLevel:
            setValue("gap-file/compression-level", value, -1);
            return;
    }

    m_return_if_reached();
}

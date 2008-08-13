#ifndef GGAP_OPTS_T_H
#define GGAP_OPTS_T_H

#include "ggap/opts.h"

namespace ggap {
namespace opt_parser_test {

struct Data {
    bool bval;
    QString sval1;
    QString sval2;

    Data(bool b = false, const QString &s1 = QString(), const QString &s2 = QString()) :
        bval(b), sval1(s1), sval2(s2)
    {
    }

    bool operator == (const Data &other) const
    {
        return bval == other.bval &&
               sval1 == other.sval1 &&
               sval2 == other.sval2;
    }
};

}
}

Q_DECLARE_METATYPE(ggap::opt_parser_test::Data)

namespace ggap {

class OptsParserTest : public QObject {
    Q_OBJECT

    bool parse(OptParser &opts, const QStringList &argv, QStringList *ret_argv, QString *errors)
    {
        QList<int> ret_idx;
        if (!opts.parse(argv, &ret_idx, errors))
            return false;
        if (ret_argv)
        {
            QStringList ret;
            foreach (int idx, ret_idx)
                ret << argv.at(idx);
            *ret_argv = ret;
        }
        return true;
    }

    typedef ggap::opt_parser_test::Data Data;

private Q_SLOTS:
    void testQt_data()
    {
        QTest::addColumn<QStringList>("args");
        QTest::addColumn<QStringList>("retArgs");
        QTest::addColumn<QStringList>("positional");

        QStringList args, ret, pos;

        QTest::newRow("1") << args << ret << pos;

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain";
        pos << "afile" << "fileagain";
        QTest::newRow("2") << args << ret << pos;

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--" << "-whatever" << "-help" << "--help";
        pos << "afile" << "fileagain" << "-whatever" << "-help" << "--help";
        QTest::newRow("3") << args << ret << pos;

        args.clear(); ret.clear(); pos.clear();
        args << "-style" << "some" << "-style=some" << "-widgetcount" << "afile" << "-reverse";
        ret << "-style" << "some" << "-style=some" << "-widgetcount" << "-reverse";
        pos << "afile";
        QTest::newRow("4") << args << ret << pos;

        args.clear(); ret.clear(); pos.clear();
        args << "-style" << "some" << "-widgetcount" << "afile" << "-reverse";
        ret << "-style" << "some" << "-widgetcount" << "-reverse";
        pos << "afile";
        QTest::newRow("5") << args << ret << pos;
    }

    void testQt()
    {
        QFETCH(QStringList, args);
        QFETCH(QStringList, retArgs);
        QFETCH(QStringList, positional);

        OptParser opts("test");

        QStringList argv;
        QStringList retArgv;

        argv << "test" << args;
        retArgv << "test" << retArgs;

        QString errors;
        QStringList actualRetArgv;
        bool result = parse(opts, argv, &actualRetArgv, &errors);

        if (!result)
            std::cout << qPrintable(errors);

        QVERIFY(result);

        QCOMPARE(actualRetArgv, retArgv);
        QCOMPARE(opts.files(), positional);
    }

    void testOpts_data()
    {
        QTest::addColumn<QStringList>("args");
        QTest::addColumn<QStringList>("retArgs");
        QTest::addColumn<QStringList>("positional");
        QTest::addColumn<Data>("input");
        QTest::addColumn<Data>("output");

        QStringList args, ret, pos;

        QTest::newRow("1") << args << ret << pos << Data() << Data();

        // --void
        // --switch
        // --second[=VALUE]
        // --third=VALUE

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--void" << "--switch";
        pos << "afile" << "fileagain";
        QTest::newRow("2") << args << ret << pos << Data(false) << Data(true);

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--void";
        pos << "afile" << "fileagain";
        QTest::newRow("3") << args << ret << pos << Data(true) << Data(true);

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--second";
        pos << "afile" << "fileagain";
        QTest::newRow("4") << args << ret << pos << Data(false, "a") << Data(false, "a");

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--second=foobar";
        pos << "afile" << "fileagain";
        QTest::newRow("5") << args << ret << pos << Data(true, "a") << Data(true, "foobar");

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--second" << "foobar";
        pos << "afile" << "fileagain" << "foobar";
        QTest::newRow("6") << args << ret << pos << Data(true, "a") << Data(true, "a");

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "fileagain" << "--third" << "foobar";
        pos << "afile" << "fileagain";
        QTest::newRow("7") << args << ret << pos << Data(true, "", "a") << Data(true, "", "foobar");

        args.clear(); ret.clear(); pos.clear();
        args << "afile" << "-style=some" << "fileagain" << "--third=blah" << "foobar";
        ret << "-style=some";
        pos << "afile" << "fileagain" << "foobar";
        QTest::newRow("8") << args << ret << pos << Data(true, "", "a") << Data(true, "", "blah");
    }

    void testOpts()
    {
        QFETCH(QStringList, args);
        QFETCH(QStringList, retArgs);
        QFETCH(QStringList, positional);
        QFETCH(Data, input);
        QFETCH(Data, output);

        OptParser opts("test");

        Data data = input;
        opts.add("void", "void");
        opts.add("switch", "switch", &data.bval);
        opts.add("second", "VALUE", "opt value", &data.sval1, false);
        opts.add("third", "VALUE", "req value", &data.sval2);

        QStringList argv;
        QStringList retArgv;

        argv << "test" << args;
        retArgv << "test" << retArgs;

        QString errors;
        QStringList actualRetArgv;
        bool result = parse(opts, argv, &actualRetArgv, &errors);

        if (!result)
            std::cout << qPrintable(errors);

        QVERIFY(result);

        QCOMPARE(actualRetArgv, retArgv);
        QCOMPARE(opts.files(), positional);
        QCOMPARE(data, output);
    }

    void testFail_data()
    {
        QTest::addColumn<QStringList>("args");

        QStringList args;

        // --void
        // --switch
        // --second[=VALUE]
        // --third=VALUE

        args.clear(); args << "--a";
        QTest::newRow("1") << args;

        args.clear(); args << "-";
        QTest::newRow("2") << args;

        args.clear(); args << "-a";
        QTest::newRow("3") << args;

        args.clear(); args << "-afe";
        QTest::newRow("4") << args;

        args.clear(); args << "-style";
        QTest::newRow("5") << args;

        args.clear(); args << "-style" << "-abra";
        QTest::newRow("6") << args;

        args.clear(); args << "--third" << "-abra";
        QTest::newRow("7") << args;

        args.clear(); args << "--third" << "--";
        QTest::newRow("8") << args;

        args.clear(); args << "--third";
        QTest::newRow("9") << args;
    }

    void testFail()
    {
        QFETCH(QStringList, args);

        Data data;
        OptParser opts("test");
        opts.add("void", "void");
        opts.add("switch", "switch", &data.bval);
        opts.add("second", "VALUE", "opt value", &data.sval1, false);
        opts.add("third", "VALUE", "req value", &data.sval2);

        QStringList argv;
        argv << "test" << args;

        QString errors;
        bool result = parse(opts, argv, 0, &errors);

        QVERIFY(!result);
        QVERIFY(!errors.isEmpty());
    }
};

}

#endif // GGAP_OPTS_T_H

#ifndef GAP_PARSER_T_H
#define GAP_PARSER_T_H

#include "ggap/parser-p.h"
#include <QtTest>

namespace ggap {
namespace parser {

class ParserTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testParseResult_data()
    {
        QTest::addColumn<QString>("code");
        QTest::addColumn<Result>("result");

        QTest::newRow("1") << "foo();" << Success;
        QTest::newRow("2") << "" << Success;
        QTest::newRow("3") << "dddd" << Incomplete;
        QTest::newRow("4") << "an error" << Error;
    }

    void testParseResult()
    {
        QFETCH(QString, code);
        QFETCH(Result, result);

        Text out;
        QList<ErrorInfo> errors;
        Result actual = parse(code.split('\n'), &out, &errors);

        QCOMPARE(result, actual);
    }
};

}
}

Q_DECLARE_METATYPE(ggap::parser::Result)

#endif // GAP_PARSER_T_H

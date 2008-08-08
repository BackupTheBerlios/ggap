#include "ggap/tester.h"
#include "ggap/gws-t.h"
#include "ggap/script-t.h"
#include "ggap/parser-t.h"
#include <QtTest>

#define TEST_CLASS(Class, args) { \
    Class::UnitTest test; \
    results << QTest::qExec(&test, args); \
}

#define TEST(TestClass, args) { \
    TestClass test; \
    results << QTest::qExec(&test, args); \
}

int moo::test::Tester::exec(const QStringList &args)
{
    QList<int> results;

    TEST_CLASS(ggap::GwsReader, args);
    TEST_CLASS(ggap::GwsWriter, args);
    TEST_CLASS(ggap::WorksheetScriptProxy, args);
    TEST(ggap::parser::ParserTest, args);

    foreach (int r, results)
        if (r != 0)
            return r;
    return 0;
}

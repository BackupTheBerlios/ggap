#include "ggap/tester.h"
#include "ggap/gws-t.h"
#include "ggap/script-t.h"
#include <QtTest>

#define TEST_CLASS(Class, args) { \
    Class::UnitTest test; \
    QTest::qExec(&test, args); \
}

int moo::test::Tester::exec(const QStringList &args)
{
    TEST_CLASS(ggap::GwsReader, args);
    TEST_CLASS(ggap::GwsWriter, args);
    TEST_CLASS(ggap::WorksheetScriptProxy, args);
    return 0;
}

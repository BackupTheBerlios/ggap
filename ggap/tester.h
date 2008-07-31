#ifndef GGAP_TESTER_H
#define GGAP_TESTER_H
#ifdef MOO_ENABLE_UNIT_TESTS

#include <moo-unit-tests.h>
#include <QStringList>

namespace moo {
namespace test {

class Tester {
public:
    int exec(const QStringList &args);
};

}
}

#endif // MOO_ENABLE_UNIT_TESTS
#endif // GGAP_TESTER_H

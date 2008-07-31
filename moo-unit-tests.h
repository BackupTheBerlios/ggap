#ifndef MOO_UNIT_TESTS_H
#define MOO_UNIT_TESTS_H

#ifdef MOO_ENABLE_UNIT_TESTS

namespace moo {
namespace test {

class Tester;

}
}

#define MOO_DECLARE_UNIT_TESTS() \
private: \
    class UnitTest; \
    friend class UnitTest; \
    friend class moo::test::Tester;

#else // !MOO_ENABLE_UNIT_TESTS

#define MOO_DECLARE_UNIT_TESTS()

#endif // !MOO_ENABLE_UNIT_TESTS

#endif // MOO_UNIT_TESTS_H

#include <boost/test/unit_test.hpp>
#include <utils/TypeChain.hpp>

BOOST_AUTO_TEST_SUITE(TypeChainTests)

BOOST_AUTO_TEST_CASE(TypeChain_get) {
    type_chain<int, float, double> tc;

    get<double>(tc) = 100.0;
    get<float>(tc) = 10.0f;
    get<int>(tc) = 1;

    BOOST_CHECK_EQUAL(1, get<int>(tc));
    BOOST_CHECK_EQUAL(10.0f, get<float>(tc));
    BOOST_CHECK_EQUAL(100.0, get<double>(tc));
}

BOOST_AUTO_TEST_SUITE_END()

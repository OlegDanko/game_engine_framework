#include <boost/test/unit_test.hpp>
#include <utils/TypeChain.hpp>

BOOST_AUTO_TEST_SUITE(TypeChainTests)

BOOST_AUTO_TEST_CASE(TypeChain_is_type_present) {
    bool v = is_type_present_v<int, int>;
    BOOST_CHECK_EQUAL(v, true);
    v = is_type_present_v<int, float>;
    BOOST_CHECK_EQUAL(v, false);
    v = is_type_present_v<int, int, float>;
    BOOST_CHECK_EQUAL(v, true);
    v = is_type_present_v<int, float, int>;
    BOOST_CHECK_EQUAL(v, true);
    v = is_type_present_v<int, float, double>;
    BOOST_CHECK_EQUAL(v, false);
}

BOOST_AUTO_TEST_CASE(TypeChain_get) {
    type_chain<int, float, double> tc;

    get<double>(tc) = 100.0;
    get<float>(tc) = 10.0f;
    get<int>(tc) = 1;

    BOOST_CHECK_EQUAL(1, get<int>(tc));
    BOOST_CHECK_EQUAL(10.0f, get<float>(tc));
    BOOST_CHECK_EQUAL(100.0, get<double>(tc));
}

template<typename T>
struct TestStruct {
    T val;
};

BOOST_AUTO_TEST_CASE(TmplTypeChain_get) {

    tmpl_type_chain<TestStruct, int, float, double> tc;

    get<double>(tc).val = 100.0;
    get<float>(tc).val = 10.0f;
    get<int>(tc).val = 1;

    BOOST_CHECK_EQUAL(1, get<int>(tc).val);
    BOOST_CHECK_EQUAL(10.0f, get<float>(tc).val);
    BOOST_CHECK_EQUAL(100.0, get<double>(tc).val);
}

BOOST_AUTO_TEST_SUITE_END()

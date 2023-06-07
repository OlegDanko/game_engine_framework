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
using u_ptr = std::unique_ptr<T>;

BOOST_AUTO_TEST_CASE(TmplTypeChain_get) {

    tmpl_type_chain<u_ptr, int, float, double> tc;

    get<double>(tc) = std::make_unique<double>(100.0);
    get<float>(tc) = std::make_unique<float>(10.0f);
    get<int>(tc) = std::make_unique<int>(1);

    BOOST_CHECK_EQUAL(1, *get<int>(tc));
    BOOST_CHECK_EQUAL(10.0f, *get<float>(tc));
    BOOST_CHECK_EQUAL(100.0, *get<double>(tc));
}

BOOST_AUTO_TEST_CASE(TmplTypeChain_make) {
    auto chain_0 = make_tmpl_type_chain<u_ptr>(std::make_unique<int>(1));
    BOOST_CHECK_EQUAL(1, *get<int>(chain_0));
    auto chain_1 = make_tmpl_type_chain<u_ptr>(std::make_unique<int>(2),
                                               std::make_unique<float>(20.0f),
                                               std::make_unique<double>(200.0));

    BOOST_CHECK_EQUAL(2, *get<int>(chain_1));
    BOOST_CHECK_EQUAL(20.0f, *get<float>(chain_1));
    BOOST_CHECK_EQUAL(200.0, *get<double>(chain_1));
}

BOOST_AUTO_TEST_SUITE_END()

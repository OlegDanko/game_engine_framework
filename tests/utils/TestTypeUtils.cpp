#include <boost/test/unit_test.hpp>
#include <utils/TypeUtils.hpp>

BOOST_AUTO_TEST_SUITE(TypeUtilsTests)

BOOST_AUTO_TEST_CASE(TypeUtils_is_type_present) {
    BOOST_CHECK_EQUAL((is_type_present_v<int, int>), true);
    BOOST_CHECK_EQUAL((is_type_present_v<int, float>), false);
    BOOST_CHECK_EQUAL((is_type_present_v<int, int, float>), true);
    BOOST_CHECK_EQUAL((is_type_present_v<int, float, int>), true);
    BOOST_CHECK_EQUAL((is_type_present_v<int, float, double>), false);
}

BOOST_AUTO_TEST_CASE(TypeUtils_types_intersect) {
    BOOST_CHECK_EQUAL((types_intersect_v<types<int>, types<int>>), true);
    BOOST_CHECK_EQUAL((types_intersect_v<types<int>, types<float>>), false);
    BOOST_CHECK_EQUAL((types_intersect_v<types<double, int>, types<int, float>>), true);
    BOOST_CHECK_EQUAL((types_intersect_v<types<char, int>, types<float, double>>), false);
}

BOOST_AUTO_TEST_SUITE_END()

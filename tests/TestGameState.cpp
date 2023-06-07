#include <boost/test/unit_test.hpp>
#include <GameState.hpp>

BOOST_AUTO_TEST_SUITE(GameStateTests)

template<typename T>
using gen_frame_t = typename AttributeStateQueue_DefGen<T>::GenFrame;

BOOST_AUTO_TEST_CASE(GameState_case) {
    GameState<int, float, double> game_state;

    auto queue_int = game_state.get_queues<int>();

    *get<int>(queue_int).get_gen_frame().gen_attr(1) = 1;

    if(auto attr_ptr = get<int>(queue_int).get_mod_frame().get_attr(1); true) {
        BOOST_CHECK(attr_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_ptr, 1);
        *attr_ptr = 2;
    }
    if(auto attr_ptr = get<int>(queue_int).get_read_frame().read_attr(1); true) {
        BOOST_CHECK(attr_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_ptr, 2);
    }


    auto queue_f_d = game_state.get_queues<float, double>();

    *get<float>(queue_f_d).get_gen_frame().gen_attr(2) = 10.0f;
    *get<double>(queue_f_d).get_gen_frame().gen_attr(2) = 100.0;

    if(auto attr_f_ptr = get<float>(queue_f_d).get_mod_frame().get_attr(2); true) {
        BOOST_CHECK(attr_f_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_f_ptr, 10.0f);
        *attr_f_ptr = 20.0f;
    }

    if(auto attr_d_ptr = get<double>(queue_f_d).get_mod_frame().get_attr(2); true) {
        BOOST_CHECK(attr_d_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_d_ptr, 100.0);
        *attr_d_ptr = 200.0;
    }

    if(auto attr_f_ptr = get<float>(queue_f_d).get_read_frame().read_attr(2); true) {
        BOOST_CHECK(attr_f_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_f_ptr, 20.0f);
    }

    if(auto attr_d_ptr = get<double>(queue_f_d).get_read_frame().read_attr(2); true) {
        BOOST_CHECK(attr_d_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_d_ptr, 200.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

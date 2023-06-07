#include <boost/test/unit_test.hpp>
#include <GameState.hpp>

BOOST_AUTO_TEST_SUITE(GameStateTests)

template<typename T>
using gen_frame_t = typename AttributeStateQueue_DefGen<T>::GenFrame;

BOOST_AUTO_TEST_CASE(GameState_case) {
    GameState<int, float, double> game_state;
    if(auto gen_frames = game_state.get_gen_frames<int>(); true) {
        *get<int>(gen_frames).gen_attr(1) = 1;
    }
    if(auto mod_frames = game_state.get_mod_frames<int>(); true) {
        auto attr_ptr = get<int>(mod_frames).get_attr(1);
        BOOST_CHECK(attr_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_ptr, 1);
        *attr_ptr = 2;
    }
    if(auto read_frames = game_state.get_read_frames<int>(); true) {
        auto attr_ptr = get<int>(read_frames).read_attr(1);
        BOOST_CHECK(attr_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_ptr, 2);
    }
    if(auto gen_frames = game_state.get_gen_frames<float, double>(); true) {
        *get<float>(gen_frames).gen_attr(2) = 10.0f;
        *get<double>(gen_frames).gen_attr(2) = 100.0;
    }
    if(auto mod_frames = game_state.get_mod_frames<float, double>(); true) {
        auto attr_f_ptr = get<float>(mod_frames).get_attr(2);
        BOOST_CHECK(attr_f_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_f_ptr, 10.0f);
        *attr_f_ptr = 20.0f;

        auto attr_d_ptr = get<double>(mod_frames).get_attr(2);
        BOOST_CHECK(attr_d_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_d_ptr, 100.0);
        *attr_d_ptr = 200.0;
    }
    if(auto read_frames = game_state.get_read_frames<float, double>(); true) {
        auto attr_f_ptr = get<float>(read_frames).read_attr(2);
        BOOST_CHECK(attr_f_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_f_ptr, 20.0f);

        auto attr_d_ptr = get<double>(read_frames).read_attr(2);
        BOOST_CHECK(attr_d_ptr != nullptr);
        BOOST_CHECK_EQUAL(*attr_d_ptr, 200.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <game_state/GameState.hpp>

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
BOOST_AUTO_TEST_CASE(GameStateAccess_single_attr) {
    GameState<int> game_state;
    size_t id;
    if(auto frame = game_state.generator.get_frame(); true) {
        auto go = frame.gen_game_object<int>();
        id = go.get_id();
        go.get_attr<int>() = 1;
    }
    auto access_1 = game_state.get_access<types<int>, types<>>();
    if(auto frame = access_1.get_frame(); true) {
        auto go = frame.get_game_object(id);
        BOOST_CHECK_EQUAL(*go.get_attr<int>(), 1);
        *go.get_attr<int>() = 2;
    }
    auto access_2 = game_state.get_access<types<>, types<int>>();
    if(auto frame = access_2.get_frame(); true) {
        auto go = frame.get_game_object(id);
        BOOST_CHECK_EQUAL(*go.read_attr<int>(), 2);
    }
}
BOOST_AUTO_TEST_CASE(GameStateAccess_multiple_attrs) {
    GameState<int, float> game_state;
    size_t id;
    if(auto frame = game_state.generator.get_frame(); true) {
        auto go = frame.gen_game_object<int, float>();
        id = go.get_id();
        go.get_attr<int>() = 1;
        go.get_attr<float>() = 10.0f;
    }
    auto access_i = game_state.get_access<types<int>, types<float>>();
    if(auto frame = access_i.get_frame(); true) {
        auto go = frame.get_game_object(id);
        BOOST_CHECK_EQUAL(*go.read_attr<float>(), 10.0f);
        *go.get_attr<int>() = 2;
    }
    auto access_f = game_state.get_access<types<float>, types<int>>();
    if(auto frame = access_f.get_frame(); true) {
        auto go = frame.get_game_object(id);
        BOOST_CHECK_EQUAL(*go.read_attr<int>(), 2);
        *go.get_attr<float>() = 20.0f;
    }
    auto access_read = game_state.get_access<types<>, types<int, float>>();
    if(auto frame = access_read.get_frame(); true) {
        auto go = frame.get_game_object(id);
        BOOST_CHECK_EQUAL(*go.read_attr<float>(), 20.0f);
        BOOST_CHECK_EQUAL(*go.read_attr<int>(), 2);
    }
}

BOOST_AUTO_TEST_SUITE_END()


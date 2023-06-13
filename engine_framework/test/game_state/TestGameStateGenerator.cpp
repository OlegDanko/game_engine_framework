#include <boost/test/unit_test.hpp>
#include <game_state/GameStateGenerator.hpp>
#include <fakeit.hpp>

using namespace fakeit;


struct INotifierMock {
    virtual void notify_int(size_t id, size_t frame) = 0;
    virtual void notify_float(size_t id, size_t frame) = 0;
    virtual void notify_double(size_t id, size_t frame) = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_int() = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_float() = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_double() = 0;
    virtual void dispatch() = 0;
};

struct GameStateSGeneratorTestStruct{};
using gs_defs = GameState_defs<GameStateSGeneratorTestStruct, int, float, double>;

template<>
struct gs_defs::GameStateGenNotifier {
    Mock<INotifierMock>& mock;
    template<typename T> void notify(size_t id, size_t frame);
    template<typename T> IEventApplier<size_t, size_t>& get_event_source();
    void dispatch() { mock.get().dispatch(); }
};

using Notifier_t = gs_defs::GameStateGenNotifier;

template<>
void Notifier_t::notify<int>(size_t id, size_t frame) {
    mock.get().notify_int(id, frame);
}
template<>
void Notifier_t::notify<float>(size_t id, size_t frame) {
    mock.get().notify_float(id, frame);
}
template<>
void Notifier_t::notify<double>(size_t id, size_t frame) {
    mock.get().notify_double(id, frame);
}

template<>
IEventApplier<size_t, size_t>& Notifier_t::get_event_source<int>() {
    return mock.get().get_event_source_int();
}
template<>
IEventApplier<size_t, size_t>& Notifier_t::get_event_source<float>() {
    return mock.get().get_event_source_float();
}
template<>
IEventApplier<size_t, size_t>& Notifier_t::get_event_source<double>() {
    return mock.get().get_event_source_double();
}

BOOST_AUTO_TEST_SUITE(GameStateGeneratorTests)

BOOST_AUTO_TEST_CASE(GameStateGenerator_case) {
    gs_defs::queues_t queues;
    Mock<INotifierMock> notifier_mock;
    Mock<IEventApplier<size_t, size_t>> e_int, e_float, e_double;

    gs_defs::GameStateGenerator gs(queues,
                                   Notifier_t{notifier_mock});

    When(Method(e_int, get_id)).Return(1);
    When(Method(e_float, get_id)).Return(2);
    When(Method(e_double, get_id)).Return(3);
    When(Method(notifier_mock, get_event_source_int)).Return(e_int.get());
    When(Method(notifier_mock, get_event_source_float)).Return(e_float.get());
    When(Method(notifier_mock, get_event_source_double)).Return(e_double.get());

    BOOST_CHECK_EQUAL(gs.get_gen_event_source<int>().get_id(), 1);
    BOOST_CHECK_EQUAL(gs.get_gen_event_source<float>().get_id(), 2);
    BOOST_CHECK_EQUAL(gs.get_gen_event_source<double>().get_id(), 3);

    if(auto frame = gs.get_frame(); true) {
        Fake(Method(notifier_mock, dispatch));
        Fake(Method(notifier_mock, notify_int));
        Fake(Method(notifier_mock, notify_float));
        Fake(Method(notifier_mock, notify_double));

        auto go_1 = frame.gen_game_object<int, float>();

        Verify(Method(notifier_mock, notify_int).Using(go_1.get_id(), (size_t)1)).Once();
        Verify(Method(notifier_mock, notify_float).Using(go_1.get_id(), (size_t)1)).Once();
        Verify(Method(notifier_mock, notify_double).Using(go_1.get_id(), (size_t)1)).Never();

        auto go_2 = frame.gen_game_object<float, double>();
        BOOST_CHECK(go_1.get_id() != go_2.get_id());

        Verify(Method(notifier_mock, notify_int).Using(go_2.get_id(), (size_t)1)).Never();
        Verify(Method(notifier_mock, notify_float).Using(go_2.get_id(), (size_t)1)).Once();
        Verify(Method(notifier_mock, notify_double).Using(go_2.get_id(), (size_t)1)).Once();

        Verify(Method(notifier_mock, dispatch)).Never();
    }
    Verify(Method(notifier_mock, dispatch)).Once();

    if(auto frame = gs.get_frame(); true) {
        auto go = frame.gen_game_object<int, float, double>();

        Verify(Method(notifier_mock, notify_int).Using(go.get_id(), (size_t)2)).Once();
        Verify(Method(notifier_mock, notify_float).Using(go.get_id(), (size_t)2)).Once();
        Verify(Method(notifier_mock, notify_double).Using(go.get_id(), (size_t)2)).Once();
    }
}

BOOST_AUTO_TEST_SUITE_END()

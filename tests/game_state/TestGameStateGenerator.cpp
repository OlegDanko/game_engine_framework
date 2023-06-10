#include <boost/test/unit_test.hpp>
#include <game_state/GameStateGenerator.hpp>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(GameStateGeneratorTests)

struct INotifierMock {
    virtual void notify_int(size_t id, size_t frame) = 0;
    virtual void notify_float(size_t id, size_t frame) = 0;
    virtual void notify_double(size_t id, size_t frame) = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_int() = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_float() = 0;
    virtual IEventApplier<size_t, size_t>& get_event_source_double() = 0;
    virtual void dispatch() = 0;
};

struct NotifierMockWrapper {
    Mock<INotifierMock>& mock;
    template<typename T> void notify(size_t id, size_t frame);
    template<typename T> IEventApplier<size_t, size_t>& get_event_source();
    void dispatch() { mock.get().dispatch(); }
};
template<>
void NotifierMockWrapper::notify<int>(size_t id, size_t frame) {
    mock.get().notify_int(id, frame);
}
template<>
void NotifierMockWrapper::notify<float>(size_t id, size_t frame) {
    mock.get().notify_float(id, frame);
}
template<>
void NotifierMockWrapper::notify<double>(size_t id, size_t frame) {
    mock.get().notify_double(id, frame);
}
template<>
IEventApplier<size_t, size_t>& NotifierMockWrapper::get_event_source<int>() {
    return mock.get().get_event_source_int();
}
template<>
IEventApplier<size_t, size_t>& NotifierMockWrapper::get_event_source<float>() {
    return mock.get().get_event_source_float();
}
template<>
IEventApplier<size_t, size_t>& NotifierMockWrapper::get_event_source<double>() {
    return mock.get().get_event_source_double();
}

BOOST_AUTO_TEST_CASE(GameStateGenerator_case) {
    using gs_defs = GameState_defs<int, float, double>;
    gs_defs::queues_t queues;
    Mock<INotifierMock> notifier_mock;
    Mock<IEventApplier<size_t, size_t>> e_int, e_float, e_double;

    gs_defs::GameStateGenerator gs(queues,
                                   NotifierMockWrapper{notifier_mock});

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

BOOST_AUTO_TEST_CASE(GameStateGenNotifier_case) {
    Mock<IEventTicketCourier> courier_mock;
    Fake(Dtor(courier_mock));

    using gs_defs = GameState_defs<int, float, double>;
    gs_defs::GameStateGenNotifier notifier(
        std::unique_ptr<IEventTicketCourier>(&courier_mock.get()));

    auto id_i = notifier.get_event_source<int>().get_id();
    auto id_f = notifier.get_event_source<float>().get_id();
    auto id_d = notifier.get_event_source<double>().get_id();

    std::shared_ptr<IEventTicket> ticket_i, ticket_f, ticket_d;

    auto expect_notification = [&](size_t id_x, std::shared_ptr<IEventTicket>& t) {
        When(Method(courier_mock, on_ticket)).Do([&t, id_x](auto id, auto ticket){
            BOOST_CHECK_EQUAL(id, id_x);
            t = std::move(ticket);
        });
    };

    expect_notification(id_i, ticket_i);
    notifier.notify<int>(1, 2);
    Verify(Method(courier_mock, on_ticket)).Once();
    courier_mock.ClearInvocationHistory();

    expect_notification(id_f, ticket_f);
    notifier.notify<float>(3, 4);
    Verify(Method(courier_mock, on_ticket)).Once();
    courier_mock.ClearInvocationHistory();

    expect_notification(id_d, ticket_d);
    notifier.notify<double>(5, 6);
    Verify(Method(courier_mock, on_ticket)).Once();
    courier_mock.ClearInvocationHistory();

    auto check_apply = [&](auto& event_src, auto& ticket, size_t g, size_t f) {
        bool applied = false;
        event_src.apply(*ticket, [=, &applied](auto go, auto frame){
            BOOST_CHECK_EQUAL(go, g);
            BOOST_CHECK_EQUAL(frame, f);
            applied = true;
        });
        BOOST_CHECK(applied);
    };

    check_apply(notifier.get_event_source<int>(), ticket_i, 1, 2);
    check_apply(notifier.get_event_source<float>(), ticket_f, 3, 4);
    check_apply(notifier.get_event_source<double>(), ticket_d, 5, 6);
}

BOOST_AUTO_TEST_SUITE_END()

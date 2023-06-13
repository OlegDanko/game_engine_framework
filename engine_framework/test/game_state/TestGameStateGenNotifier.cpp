#include <boost/test/unit_test.hpp>
#include <game_state/GameStateGenNotifier.hpp>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(GameStateGenNotifierTests)

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

#include <boost/test/unit_test.hpp>
#include <events/EventSource.hpp>

#include <memory>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(EventSourceTests)

BOOST_AUTO_TEST_CASE(EventSource_case) {
    Mock<IEventTicketCourier> courier_mock;

    EventSource<int, double> src(123, courier_mock.get());

    BOOST_CHECK_EQUAL(&src.get_receiver_registry(), &courier_mock.get());

    std::shared_ptr<IEventTicket> ticket;

    When(Method(courier_mock, on_ticket)).Do([&](auto src_id, auto t){
        BOOST_CHECK_EQUAL(src_id, 123);
        ticket = std::move(t);
    });

    bool applied = false;
    auto apply_fn = [&](auto i, auto d) {
        applied = true;
        BOOST_CHECK_EQUAL(i, 10);
        BOOST_CHECK_EQUAL(d, 100.0);
    };

    src.apply(1, apply_fn);
    BOOST_CHECK(!applied);

    Verify(Method(courier_mock, on_ticket)).Never();
    src.create(10, 100.0);
    Verify(Method(courier_mock, on_ticket)).Once();

    auto ticket_id = ticket->get_id();

    src.apply(ticket_id, apply_fn);
    BOOST_CHECK(applied);

    courier_mock.Reset();
    applied = false;
    ticket = nullptr;

    src.apply(ticket_id, apply_fn);
    BOOST_CHECK(!applied);
}

BOOST_AUTO_TEST_SUITE_END()

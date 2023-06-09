#include <boost/test/unit_test.hpp>
#include <events/EventSource.hpp>

#include <memory>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(EventSourceTests)

BOOST_AUTO_TEST_CASE(EventSource_case) {
    Mock<IEventTicketCourier> courier_mock;
    Mock<IEventTicket> ticket_mock;

    EventSource<int, double> src(123, courier_mock.get());
    BOOST_CHECK_EQUAL(&src.get_receiver_registry(), &courier_mock.get());

    // Save the ticket when an event is created
    std::shared_ptr<IEventTicket> ticket;
    When(Method(courier_mock, on_ticket)).Do([&](auto src_id, auto t){
        BOOST_CHECK_EQUAL(src_id, 123);
        ticket = std::move(t);
    });

    // Chech whether a callback was applied
    bool applied = false;
    auto apply_fn = [&](auto i, auto d) {
        applied = true;
        BOOST_CHECK_EQUAL(i, 10);
        BOOST_CHECK_EQUAL(d, 100.0);
    };

    // Apply before event was created shouldn't invoke
    When(Method(ticket_mock, get_id)).Return(2);
    When(Method(ticket_mock, get_listener_ptr)).Return(nullptr);
    src.apply(ticket_mock.get(), apply_fn);
    BOOST_CHECK(!applied);

    // create method should call IEventTicketCourier::on_ticket
    Verify(Method(courier_mock, on_ticket)).Never();
    src.create(10, 100.0);
    Verify(Method(courier_mock, on_ticket)).Once();

    courier_mock.Reset();
    auto ticket_id = ticket->get_id();
    auto listener_ptr = ticket->get_listener_ptr();

    // Apply with wrong id shouldn't invoke
    When(Method(ticket_mock, get_id)).Return(2);
    When(Method(ticket_mock, get_listener_ptr)).Return(listener_ptr);
    src.apply(ticket_mock.get(), apply_fn);
    BOOST_CHECK(!applied);

    // Apply with wrong listener ptr shouldn't invoke
    When(Method(ticket_mock, get_id)).Return(ticket_id);
    When(Method(ticket_mock, get_listener_ptr)).Return(nullptr);
    src.apply(ticket_mock.get(), apply_fn);
    BOOST_CHECK(!applied);

    // Apply with correct id and listener ptr should invoke
    When(Method(ticket_mock, get_id)).Return(ticket_id);
    When(Method(ticket_mock, get_listener_ptr)).Return(listener_ptr);
    src.apply(ticket_mock.get(), apply_fn);
    BOOST_CHECK(applied);

    applied = false;
    ticket = nullptr;

    // Apply after ticket is deleted shouldn't invoke
    When(Method(ticket_mock, get_id)).Return(ticket_id);
    When(Method(ticket_mock, get_listener_ptr)).Return(listener_ptr);
    src.apply(ticket_mock.get(), apply_fn);
    BOOST_CHECK(!applied);
}

BOOST_AUTO_TEST_SUITE_END()

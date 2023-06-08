#include <boost/test/unit_test.hpp>
#include <events/EventTicket.hpp>

#include <memory>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(EventTicketTests)

BOOST_AUTO_TEST_CASE(EventTicket_case) {
    Mock<ITicketClosedListener> listener;
    Fake(Method(listener, on_ticket_closed));

    auto ticket_1 = std::make_unique<EventTicket>(1, listener.get());
    auto ticket_2 = std::make_unique<EventTicket>(2, listener.get());

    BOOST_CHECK_EQUAL(ticket_1->get_id(), 1);
    BOOST_CHECK_EQUAL(ticket_2->get_id(), 2);

    Verify(Method(listener, on_ticket_closed)).Never();

    ticket_1 = nullptr;
    Verify(Method(listener, on_ticket_closed).Using(1)).Once();
    Verify(Method(listener, on_ticket_closed).Using(2)).Never();

    ticket_2 = nullptr;
    Verify(Method(listener, on_ticket_closed).Using(2)).Once();
}

BOOST_AUTO_TEST_SUITE_END()

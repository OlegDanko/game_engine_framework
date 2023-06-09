#include <boost/test/unit_test.hpp>
#include <events/EventTicketCourier.hpp>
#include <events/IEventTicket.hpp>
#include <events/IEventTicketReceiver.hpp>

#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(EventTicketCourierTests)

auto shared_from_ticket_mock(Mock<IEventTicket>& mock) {
    Fake(Dtor(mock));
    return std::shared_ptr<IEventTicket>(&mock.get());
};

BOOST_AUTO_TEST_CASE(EventTicketCourier_case) {
    Mock<IEventTicketFrame> frame;
    Fake(Dtor(frame));
    Fake(Method(frame, add_ticket));

    Mock<IEventTicketReceiver> receiver;
    When(Method(receiver, get_frame)).AlwaysDo([&]{
        return std::unique_ptr<IEventTicketFrame>{&frame.get()};
    });

    Mock<IEventTicket> tm_1, tm_2, tm_3;
    auto t_1 = shared_from_ticket_mock(tm_1);
    auto t_2 = shared_from_ticket_mock(tm_2);
    auto t_3 = shared_from_ticket_mock(tm_3);

    EventTicketCourier courier;

    // Tickets added before dispatch shouldn't be sent to a registered receiver
    courier.on_ticket(1, t_1);
    courier.register_receiver(1, &receiver.get());
    courier.on_ticket(1, t_2);
    courier.dispatch();

    // Tickets added after dispatch should be sent
    courier.on_ticket(1, t_3);
    courier.dispatch();

    Verify(Method(frame, add_ticket).Using(1, t_1)).Never();
    Verify(Method(frame, add_ticket).Using(1, t_2)).Never();
    Verify(Method(frame, add_ticket).Using(1, t_3)).Once();

    frame.Reset();
}

BOOST_AUTO_TEST_CASE(EventTicketCourier_multipleSendRecv) {
    Mock<IEventTicketFrame> frame_1;
    Mock<IEventTicketFrame> frame_2;
    Fake(Dtor(frame_1));
    Fake(Dtor(frame_2));
    Fake(Method(frame_1, add_ticket));
    Fake(Method(frame_2, add_ticket));

    Mock<IEventTicketReceiver> receiver_1;
    Mock<IEventTicketReceiver> receiver_2;
    When(Method(receiver_1, get_frame)).AlwaysDo([&]{
        return std::unique_ptr<IEventTicketFrame>{&frame_1.get()};
    });
    When(Method(receiver_2, get_frame)).AlwaysDo([&]{
        return std::unique_ptr<IEventTicketFrame>{&frame_2.get()};
    });

    auto init_ticket = [](Mock<IEventTicket>& mock, size_t id) {
        Fake(Dtor(mock));
        When(Method(mock, get_id)).AlwaysReturn(id);
        return std::shared_ptr<IEventTicket>(&mock.get());
    };
    Mock<IEventTicket> tm_1, tm_2, tm_3;
    auto t_1 = shared_from_ticket_mock(tm_1);
    auto t_2 = shared_from_ticket_mock(tm_2);
    auto t_3 = shared_from_ticket_mock(tm_3);

    EventTicketCourier courier;

    courier.register_receiver(1, &receiver_1.get());
    courier.register_receiver(2, &receiver_1.get());

    courier.register_receiver(2, &receiver_2.get());
    courier.register_receiver(3, &receiver_2.get());
    courier.dispatch();

    courier.on_ticket(1, t_1);
    courier.on_ticket(2, t_2);
    courier.on_ticket(3, t_3);
    courier.dispatch();

    Verify(Method(frame_1, add_ticket).Using(1, t_1)).Once();
    Verify(Method(frame_1, add_ticket).Using(2, t_2)).Once();
    Verify(Method(frame_1, add_ticket).Using(3, t_3)).Never();

    Verify(Method(frame_2, add_ticket).Using(1, t_1)).Never();
    Verify(Method(frame_2, add_ticket).Using(2, t_2)).Once();
    Verify(Method(frame_2, add_ticket).Using(3, t_3)).Once();

    frame_1.Reset();
    frame_2.Reset();
}

BOOST_AUTO_TEST_SUITE_END()

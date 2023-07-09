#include <boost/test/unit_test.hpp>
#include <events/EventReceiver.hpp>
#include <events/IEventTicket.hpp>

#include <memory>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(EventReceiverTests)

BOOST_AUTO_TEST_CASE(EventReceiver_case) {
    Mock<IEventApplier<int, float>> event_applier;
    Fake(Method(event_applier, register_receiver));
    When(Method(event_applier, get_id)).Return(1);
    Fake(Method(event_applier, apply));

    Mock<IEventTicket> ticket;
    Fake(Dtor(ticket));
    std::shared_ptr<IEventTicket> ticket_s_ptr(&ticket.get());

    EventReceiver recv;

    std::function<void(const int&, const float&)> callback =
        [](const int&, const float&) {};

    auto frame = recv.get_frame();
    frame->add_ticket(1, ticket_s_ptr);

    recv.register_callback(event_applier.get(), callback);
    Verify(Method(event_applier, register_receiver).Using(&recv)).Once();

    frame = nullptr;
    recv.serve_events();
    Verify(Method(event_applier, apply)).Once();
    event_applier.ClearInvocationHistory();

    frame = recv.get_frame();
    frame->add_ticket(1, ticket_s_ptr);

    recv.serve_events();
    Verify(Method(event_applier, apply)).Never();

    frame = nullptr;
    recv.serve_events();
    Verify(Method(event_applier, apply)).Once();
}

BOOST_AUTO_TEST_CASE(EventReceiver_mutiple_appliers) {
    Mock<IEventApplier<int, float>> event_applier_1, event_applier_2;
    Fake(Method(event_applier_1, register_receiver));
    Fake(Method(event_applier_2, register_receiver));
    When(Method(event_applier_1, get_id)).Return(1);
    When(Method(event_applier_2, get_id)).Return(2);
    Fake(Method(event_applier_1, apply));
    Fake(Method(event_applier_2, apply));

    Mock<IEventTicket> t1, t2, t3;
    Fake(Dtor(t1));
    Fake(Dtor(t2));
    Fake(Dtor(t3));
    std::shared_ptr<IEventTicket> t1_sptr(&t1.get());
    std::shared_ptr<IEventTicket> t2_sptr(&t2.get());
    std::shared_ptr<IEventTicket> t3_sptr(&t3.get());

    EventReceiver recv;

    using cb_t = std::function<void(const int&, const float&)>;
    cb_t callback = [](const int&, const float&) {};

    recv.register_callback(event_applier_1.get(), callback);
    recv.register_callback(event_applier_2.get(), callback);

    auto frame = recv.get_frame();
    frame->add_ticket(1, t1_sptr);
    frame->add_ticket(2, t2_sptr);
    frame->add_ticket(2, t3_sptr);
    frame = nullptr;

    recv.serve_events();

    Verify(Method(event_applier_1, apply).Matching([&](auto& t, auto&){
        return &t == t1_sptr.get();
    })).Once();
    Verify(Method(event_applier_1, apply).Matching([&](auto& t, auto&){
        return &t == t2_sptr.get();
    })).Never();
    Verify(Method(event_applier_1, apply).Matching([&](auto& t, auto&){
        return &t == t3_sptr.get();
    })).Never();

    Verify(Method(event_applier_2, apply).Matching([&](auto& t, auto&){
        return &t == t1_sptr.get();
    })).Never();
    Verify(Method(event_applier_2, apply).Matching([&](auto& t, auto&){
        return &t == t2_sptr.get();
    })).Once();
    Verify(Method(event_applier_2, apply).Matching([&](auto& t, auto&){
        return &t == t3_sptr.get();
    })).Once();

    event_applier_1.Reset();
    event_applier_2.Reset();
}

BOOST_AUTO_TEST_SUITE_END()

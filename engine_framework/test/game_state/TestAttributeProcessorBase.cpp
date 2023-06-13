#include <boost/test/unit_test.hpp>
#include <events/IEventTicket.hpp>
#include <events/EventSource.hpp>
#include <game_state/AttributeProcessorBase.hpp>

#include <fakeit.hpp>

using namespace fakeit;

struct AttributeProcessorBaseTest{};
using gs_defs = GameState_defs<AttributeProcessorBaseTest, int, float>;

struct IGameObjectMock {
    virtual void mark(size_t) = 0;
    virtual size_t get_id() = 0;
    virtual ~IGameObjectMock() = default;
};
struct GameObjectWrapper {
    IGameObjectMock& mock;
    void mark(size_t s) {mock.mark(s);}
    size_t get_id() { return mock.get_id(); }
};

struct IFrameMock {
    virtual void mark(size_t) = 0;
    virtual GameObjectWrapper get_game_object(size_t id) = 0;
    virtual ~IFrameMock() = default;
};
struct FrameMockWrapper {
    using GameStateObject = GameObjectWrapper;
    IFrameMock& mock;
    GameStateObject get_game_object(size_t id) { return mock.get_game_object(id); }
    void mark(size_t s) { mock.mark(s); }
};

struct IAccessMock {
    virtual FrameMockWrapper get_frame() = 0;
    virtual ~IAccessMock() = default;
};

template<>
struct gs_defs::GameState {
    IAccessMock& access_mock;
};

template<> template<>
struct gs_defs::GameStateAccess<types<int>, types<float>> {
    using Frame = FrameMockWrapper;
    IAccessMock& access_mock;

    GameStateAccess(GameState& gs)
        : access_mock(gs.access_mock)
    {}

    FrameMockWrapper get_frame() {
        return access_mock.get_frame();
    }
};

BOOST_AUTO_TEST_SUITE(AttributeProcessorBaseTests)

using GameState_t = gs_defs::GameState;
using Access_t = gs_defs::GameStateAccess<types<int>, types<float>>;

BOOST_AUTO_TEST_CASE(AttributeProcessorBase_update) {
    class TestProcessor : public gs_defs::AttributeProcessorBase<types<int>, types<float>> {
        void update_attributes(frame_t& f) override {update_attributes_fn(f);}
        void update() override {update_fn();}
    public:
        std::function<void(frame_t&)> update_attributes_fn = [](auto&){};
        std::function<void()> update_fn = [](){};
        TestProcessor(GameState_t gs) : Base_t(gs) {}
    };

    Mock<IFrameMock> frame_mock;
    Fake(Method(frame_mock, mark));
    Mock<IAccessMock> access_mock;
    When(Method(access_mock, get_frame)).Return(FrameMockWrapper{frame_mock.get()});
    GameState_t gs{access_mock.get()};
    TestProcessor processor(gs);

    processor.update_attributes_fn = [](auto& frame){ frame.mark(1); };

    int applied = 0;
    processor.update_fn = [&](){ applied++; };
    processor.update__();

    BOOST_CHECK_EQUAL(applied, 1);

    Verify(Method(access_mock, get_frame)).Once();
    Verify(Method(frame_mock, mark)).Once();
    Verify(Method(frame_mock, mark).Using((size_t)1)).Once();
}

BOOST_AUTO_TEST_CASE(AttributeProcessorBase_register_callbacks) {
    class TestProcessor : public gs_defs::AttributeProcessorBase<types<int>, types<float>> {
    public:
        TestProcessor(GameState_t gs) : Base_t(gs) {}
    };
    Mock<IGameObjectMock> go_mock;
    Fake(Method(go_mock, mark));
    Mock<IFrameMock> frame_mock;
    Fake(Method(frame_mock, mark));
    When(Method(frame_mock, get_game_object)).Return(GameObjectWrapper{go_mock.get()});
    Mock<IAccessMock> access_mock;
    When(Method(access_mock, get_frame)).AlwaysReturn(FrameMockWrapper{frame_mock.get()});

    GameState_t gs{access_mock.get()};
    TestProcessor processor(gs);

    IEventTicketReceiver* recv_1, *recv_2;
    Mock<IEventApplier<int>> frame_event;
    When(Method(frame_event, get_id)).Return((size_t)1);
    When(Method(frame_event, register_receiver)).Do([&](auto*r){recv_1 = r;});
    When(Method(frame_event, apply)).Do([](auto&, auto cb){ cb(1); });
    Mock<IEventApplier<size_t, int>> game_object_event;
    When(Method(game_object_event, get_id)).Return((size_t)2);
    When(Method(game_object_event, register_receiver)).Do([&](auto*r){recv_2 = r;});
    When(Method(game_object_event, apply)).Do([](auto&, auto cb){ cb(2, 3); });

    processor.register_callback(frame_event.get(), [](auto& frame, int){
        frame.mark(1);
    });
    processor.register_object_callback(game_object_event.get(), [](auto go, auto& frame, int){
        go.mark(2);
        frame.mark(2);
    });

    Mock<IEventTicket> ticket_mock;
    Fake(Dtor(ticket_mock));
    auto ticket_sptr = std::shared_ptr<IEventTicket>(&ticket_mock.get());

    recv_1->get_frame()->add_ticket(1, ticket_sptr);
    processor.update__();

    Verify(Method(frame_mock, mark)).Once();
    Verify(Method(frame_mock, mark).Using((size_t)1)).Once();
    frame_mock.ClearInvocationHistory();

    recv_2->get_frame()->add_ticket(2, ticket_sptr);
    processor.update__();

    Verify(Method(frame_mock, mark)).Once();
    Verify(Method(frame_mock, mark).Using((size_t)2)).Once();
    Verify(Method(go_mock, mark)).Once();
    Verify(Method(go_mock, mark).Using((size_t)2)).Once();

    frame_event.ClearInvocationHistory();
    game_object_event.ClearInvocationHistory();
}

BOOST_AUTO_TEST_CASE(AttributeProcessorBase_callbacks) {
    class TestProcessor : public gs_defs::AttributeProcessorBase<types<int>, types<float>> {
        void update() override {
            test_event.create(123);
        }
    public:
        EventSource<int> test_event{get_event_courier()};
        TestProcessor(GameState_t gs) : Base_t(gs) {}
    };

    Mock<IEventTicketFrame> frame;
    std::shared_ptr<IEventTicket> ticket;
    When(Method(frame, add_ticket)).Do([&](size_t, auto t){ticket = std::move(t);});
    Fake(Dtor(frame));
    Mock<IEventTicketReceiver> recv;
    When(Method(recv, get_frame)).Return(std::unique_ptr<IEventTicketFrame>(&frame.get()));

    Mock<IFrameMock> frame_mock;
    Fake(Method(frame_mock, mark));
    Mock<IAccessMock> access_mock;
    When(Method(access_mock, get_frame)).Return(FrameMockWrapper{frame_mock.get()});
    GameState_t gs{access_mock.get()};
    TestProcessor processor(gs);

    processor.test_event.register_receiver(&recv.get());

    BOOST_CHECK_EQUAL(ticket.get(), nullptr);
    processor.update__();
    BOOST_CHECK(ticket.get() != nullptr);

    Verify(Method(frame, add_ticket)).Once();

    bool applied = false;
    processor.test_event.apply(*ticket, [&](int i){
        applied = true;
        BOOST_CHECK_EQUAL(i, 123);
    });
    BOOST_CHECK(applied);

    ticket = nullptr;
    frame.ClearInvocationHistory();
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <FlatteningLifoQueue.hpp>
#include <optional>


struct TestInt { int val; };

template<>
struct flattener<TestInt> {
    void operator()(TestInt& top_saved, TestInt& bottom_dismissed) const {
        top_saved.val += bottom_dismissed.val;
    }
};


BOOST_AUTO_TEST_SUITE(FlatteningLifoQueueTests)

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    BOOST_CHECK_EQUAL(queue.observe().get().val, 1);
    const auto observer_c = queue.observe();
    BOOST_CHECK_EQUAL(observer_c.get().val, 1);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_several) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    queue.push({3});
    BOOST_CHECK_EQUAL(queue.observe().get().val, 4);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_observe_two) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    if(auto obs_0 = queue.observe(); true) {
        queue.push({3});
        BOOST_CHECK_EQUAL(queue.observe().get().val, 3);
    }
    BOOST_CHECK_EQUAL(queue.observe().get().val, 4);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_multiple_observers) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    auto obs_a_opt = std::make_optional(queue.observe());
    auto obs_b_opt = std::make_optional(queue.observe());
    queue.push({3});

    BOOST_CHECK_EQUAL(queue.observe().get().val, 3);
    obs_a_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 3);
    obs_b_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 4);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_move_observer) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    auto obs_a_opt = std::make_optional(queue.observe());
    auto obs_b_opt = std::move(obs_a_opt);
    obs_a_opt = std::nullopt;

    queue.push({3});
    BOOST_CHECK_EQUAL(queue.observe().get().val, 3);
    obs_b_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 4);
}


BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_observe_three) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    queue.push({3});
    queue.push({5});

    BOOST_CHECK_EQUAL(queue.observe().get().val, 9);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_observe_2_of_3) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    queue.push({3});
    auto obs_opt = std::make_optional(queue.observe());
    queue.push({5});

    BOOST_CHECK_EQUAL(queue.observe().get().val, 5);
    obs_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 9);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_observe_2_of_3_last) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    auto obs_0_opt = std::make_optional(queue.observe());
    queue.push({3});
    auto obs_1_opt = std::make_optional(queue.observe());
    queue.push({5});

    BOOST_CHECK_EQUAL(queue.observe().get().val, 5);

    obs_0_opt = std::nullopt;
    obs_1_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 9);
}

BOOST_AUTO_TEST_CASE(FlatteningLifoQueue_push_observe_1_of_3_last) {
    FlatteningLifoQueue<TestInt> queue;
    queue.push({1});
    auto obs_0_opt = std::make_optional(queue.observe());
    queue.push({3});
    auto obs_1_opt = std::make_optional(queue.observe());
    queue.push({5});

    BOOST_CHECK_EQUAL(queue.observe().get().val, 5);

    obs_1_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 8);
    obs_0_opt = std::nullopt;
    BOOST_CHECK_EQUAL(queue.observe().get().val, 9);
}

BOOST_AUTO_TEST_SUITE_END()

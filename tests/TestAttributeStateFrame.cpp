#include <boost/test/unit_test.hpp>
#include <Attributes.hpp>

BOOST_AUTO_TEST_SUITE(AttributeStateFrameTests)

BOOST_AUTO_TEST_CASE(AttributeStateFrame_get_attr) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;

    // frame.get_attr() always returns a value
    auto attr_ptr = frame.get_attr(1);
    BOOST_CHECK(attr_ptr != nullptr);

    // frame.get_attr() returns a pointer to the same variable each time
    attr_ptr->var = 1;
    BOOST_CHECK_EQUAL(frame.get_attr(1)->var, 1);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_get_attr_const_cast) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;

    frame.get_attr(1)->var = 1;

    const auto& frame_const = frame;
    BOOST_CHECK_EQUAL(frame_const.get_attr(1)->var, 1);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.get_attr(1)->var = 1;

    auto frame_1 = frame_t::spawn(frame_0);

    auto attr_ptr = frame_1.get_attr(1);
    BOOST_CHECK(attr_ptr != nullptr);
    BOOST_CHECK_EQUAL(attr_ptr->var, 1);

    // Non-functional requirement - frame_1.get_attr() returns a value distinct from frame_0.get_attr()
    frame_0.get_attr(1)->var = 2;
    BOOST_CHECK_EQUAL(frame_1.get_attr(1)->var, 1);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn_const) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.get_attr(1)->var = 1;

    const auto frame_1 = frame_t::spawn(frame_0);

    auto attr_ptr = frame_1.get_attr(1);
    BOOST_CHECK(attr_ptr != nullptr);
    BOOST_CHECK_EQUAL(attr_ptr->var, 1);

    // Non-functional requirement - frame_1.get_attr() returns a value distinct from frame_0.get_attr()
    frame_0.get_attr(1)->var = 2;
    BOOST_CHECK_EQUAL(frame_1.get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn_chain) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.get_attr(1)->var = 1;

    auto frame_1 = frame_t::spawn(frame_0);
    const auto frame_2 = frame_t::spawn(frame_1);

    BOOST_CHECK_EQUAL(frame_2.get_attr(1)->var, 1);

    // Non-functional requirement - frame_2.get_attr() points to the value in frame_0
    frame_0.get_attr(1)->var = 2;
    BOOST_CHECK_EQUAL(frame_2.get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn_const_chain_mod) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.get_attr(1)->var = 1;

    auto frame_1 = frame_t::spawn(frame_0);
    frame_1.get_attr(1)->var = 2;
    const auto frame_2 = frame_t::spawn(frame_1);

    // frame_2.get_attr() points to the value modified in frame_1
    BOOST_CHECK_EQUAL(frame_2.get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_merge) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;
    auto frame_0 = std::make_unique<frame_t>();
    frame_0->get_attr(1)->var = 1;

    auto frame_1 = std::make_unique<frame_t>(std::move(frame_t::spawn(*frame_0)));
    frame_0->get_attr(1)->var = 2;

    frame_t::merge(*frame_1, *frame_0);
    frame_0 = nullptr;

    BOOST_CHECK_EQUAL(frame_1->get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_merge_mod) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;
    auto frame_0 = std::make_unique<frame_t>();
    frame_0->get_attr(1)->var = 1;

    auto frame_1 = std::make_unique<frame_t>(std::move(frame_t::spawn(*frame_0)));
    frame_1->get_attr(1)->var = 2;

    frame_t::merge(*frame_1, *frame_0);
    frame_0 = nullptr;

    BOOST_CHECK_EQUAL(frame_1->get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_SUITE_END()

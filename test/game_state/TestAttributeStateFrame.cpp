#include <boost/test/unit_test.hpp>
#include <game_state/AttributeStateFrame.hpp>
#include <game_state/AttributeGeneratorDefault.hpp>

BOOST_AUTO_TEST_SUITE(AttributeStateFrameTests)

BOOST_AUTO_TEST_CASE(AttributeStateFrame_get_attr_nullptr) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;

    // Returns null - attribute with this id doesn't exist
    BOOST_CHECK(frame.get_attr(1) == nullptr);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_double_gen) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;

    BOOST_CHECK(frame.gen_attr(1) != nullptr);
    // can't create several objects with the same id
    BOOST_CHECK(frame.gen_attr(1) == nullptr);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_gen_get_read_attr) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;
    frame_t frame;

    BOOST_CHECK(frame.get_attr(1) == nullptr);
    BOOST_CHECK(frame.read_attr(1) == nullptr);

    frame.gen_attr(1)->var = 1;

    auto frame_2 = frame_t::spawn(frame);

    BOOST_CHECK_EQUAL(frame_2.read_attr(1)->var, 1);
    BOOST_CHECK_EQUAL(frame_2.get_attr(1)->var, 1);

}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_multiple_attrs) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;
    frame_t frame;

    BOOST_CHECK_EQUAL(frame.get_attr(1), nullptr);
    BOOST_CHECK_EQUAL(frame.get_attr(2), nullptr);

    frame.gen_attr(1)->var = 1;
    frame.gen_attr(2)->var = 2;

    BOOST_CHECK_EQUAL(frame.read_attr(1), nullptr);
    BOOST_CHECK_EQUAL(frame.read_attr(2), nullptr);
    BOOST_CHECK_EQUAL(frame.get_attr(1)->var, 1);
    BOOST_CHECK_EQUAL(frame.get_attr(2)->var, 2);

    auto frame_2 = frame_t::spawn(frame);
    BOOST_CHECK_EQUAL(frame_2.read_attr(1)->var, 1);
    BOOST_CHECK_EQUAL(frame_2.read_attr(2)->var, 2);
    BOOST_CHECK_EQUAL(frame_2.get_attr(1)->var, 1);
    BOOST_CHECK_EQUAL(frame_2.get_attr(2)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_get_attr_const_cast) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;

    frame.gen_attr(1)->var = 1;

    const auto& frame_const = frame;
    BOOST_CHECK_EQUAL(frame_const.get_attr(1)->var, 1);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_get_attr_const_cast_nullptr) {
    struct TestAttr { int var{0}; };
    AttributeStateFrame_DefGen<TestAttr> frame;
    const auto& frame_const = frame;
    BOOST_CHECK_EQUAL(frame_const.read_attr(1), nullptr);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.gen_attr(1)->var = 1;

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
    frame_0.gen_attr(1)->var = 1;

    const auto frame_1 = frame_t::spawn(frame_0);

    auto attr_ptr = frame_1.read_attr(1);
    BOOST_CHECK(attr_ptr != nullptr);
    BOOST_CHECK_EQUAL(attr_ptr->var, 1);

    // Non-functional requirement - frame_1.get_attr() returns a value distinct from frame_0.get_attr()
    frame_0.get_attr(1)->var = 2;
    BOOST_CHECK_EQUAL(frame_1.read_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn_chain) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.gen_attr(1)->var = 1;

    auto frame_1 = frame_t::spawn(frame_0);
    const auto frame_2 = frame_t::spawn(frame_1);

    BOOST_CHECK_EQUAL(frame_2.read_attr(1)->var, 1);

    // Non-functional requirement - frame_2.get_attr() points to the value in frame_0
    frame_0.get_attr(1)->var = 2;
    BOOST_CHECK_EQUAL(frame_2.read_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_spawn_const_chain_mod) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;

    frame_t frame_0;
    frame_0.gen_attr(1)->var = 1;

    auto frame_1 = frame_t::spawn(frame_0);
    frame_1.get_attr(1)->var = 2;
    const auto frame_2 = frame_t::spawn(frame_1);

    // frame_2.get_attr() points to the value modified in frame_1
    BOOST_CHECK_EQUAL(frame_2.read_attr(1)->var, 2);
}

BOOST_AUTO_TEST_CASE(AttributeStateFrame_merge) {
    struct TestAttr { int var{0}; };
    using frame_t = AttributeStateFrame_DefGen<TestAttr>;
    auto frame_0 = std::make_unique<frame_t>();
    frame_0->gen_attr(1)->var = 1;

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
    frame_0->gen_attr(1)->var = 1;

    auto frame_1 = std::make_unique<frame_t>(std::move(frame_t::spawn(*frame_0)));
    frame_1->get_attr(1)->var = 2;

    frame_t::merge(*frame_1, *frame_0);
    frame_0 = nullptr;

    BOOST_CHECK_EQUAL(frame_1->get_attr(1)->var, 2);
}

BOOST_AUTO_TEST_SUITE_END()

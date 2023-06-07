#include <boost/test/unit_test.hpp>
#include <AttributeStateQueue.hpp>
#include <AttributeGeneratorDefault.hpp>
#include <thread>
#include <atomic>
#include <ranges>


BOOST_AUTO_TEST_SUITE(AttributeStateQueueTests)

BOOST_AUTO_TEST_CASE(AttributeStateQueue_regularCase) {
    struct TestAttr { int var{0}; };
    AttributeStateQueue_DefGen<TestAttr> queue;

    // before attribute created
    if(auto frame = queue.get_read_frame(); true) {
        BOOST_CHECK(frame.read_attr(1) == nullptr);
    }
    if(auto frame = queue.get_mod_frame(); true) {
        BOOST_CHECK(frame.get_attr(1) == nullptr);
    }

    // creating attribute
    if(auto frame = queue.get_gen_frame(); true) {
        auto ptr = frame.gen_attr(1);
        BOOST_CHECK(ptr != nullptr);
        ptr->var = 1;
    }
    // after attribute created
    if(auto frame = queue.get_read_frame(); true) {
        BOOST_CHECK(frame.read_attr(1) != nullptr);
        BOOST_CHECK_EQUAL(frame.read_attr(1)->var, 1);
    }

    // modifify attribute
    if(auto frame = queue.get_mod_frame(); true) {
        auto ptr = frame.get_attr(1);
        BOOST_CHECK(ptr != nullptr);
        ptr->var = 2;
    }
    // after attribute modified
    if(auto frame = queue.get_read_frame(); true) {
        BOOST_CHECK_EQUAL(frame.read_attr(1)->var, 2);
    }
}

BOOST_AUTO_TEST_CASE(AttributeStateQueue_readFramePermanence) {
    struct TestAttr { int var{0}; };
    AttributeStateQueue_DefGen<TestAttr> queue;

    if(auto frame = queue.get_gen_frame(); true) {
        frame.gen_attr(1)->var = 1;
    }
    if(auto read_frame = queue.get_read_frame(); true) {
        BOOST_CHECK_EQUAL(read_frame.read_attr(1)->var, 1);

        if(auto mod_frame = queue.get_mod_frame(); true) {
            mod_frame.get_attr(1)->var = 2;
        }
        if(auto read_frame_2 = queue.get_read_frame(); true) {
            // value accessed from a read frame created after attribute update changes accordingly
            BOOST_CHECK_EQUAL(read_frame_2.read_attr(1)->var, 2);
        }

        // value accessed from a read frame created before attribute update remains unchanged
        BOOST_CHECK_EQUAL(read_frame.read_attr(1)->var, 1);
    }
}

BOOST_AUTO_TEST_CASE(AttributeStateQueue_readFrameCompleteness) {
    struct TestAttr { int var{0}; };
    AttributeStateQueue_DefGen<TestAttr> queue;

    if(auto frame = queue.get_gen_frame(); true) {
        frame.gen_attr(1)->var = 1;
        if(auto read_frame = queue.get_read_frame(); true) {
            // Attributes created in GenFrame aren't available to read before GenFrame is destroyed
            BOOST_CHECK_EQUAL(read_frame.read_attr(1), nullptr);
        }
    }
    if(auto mod_frame = queue.get_mod_frame(); true) {
        mod_frame.get_attr(1)->var = 2;
        if(auto read_frame = queue.get_read_frame(); true) {
            // Updates in a ModFrame aren't available to read before ModFrame object is destroyed
            BOOST_CHECK_EQUAL(read_frame.read_attr(1)->var, 1);
        }
    }
}

BOOST_AUTO_TEST_CASE(AttributeStateQueue_modFrame) {
    struct TestAttr { int var{0}; };
    AttributeStateQueue_DefGen<TestAttr> queue;

    if(auto frame = queue.get_gen_frame(); true) {
        frame.gen_attr(1)->var = 1;
    }
    std::atomic_bool execution_started{false};
    std::vector<std::thread> threads;
    std::vector<int> seq;

    for(auto i : std::views::iota(0, 50)) {
        threads.push_back(std::thread([&execution_started, &queue, &seq, i]{
            execution_started.wait(false);
            if(auto frame = queue.get_mod_frame(); true) {
                seq.push_back(i);
                frame.get_attr(1)->var = i;
                seq.push_back(i);
            }
        }));
    }
    for(auto i : std::views::iota(50, 100)) {
        threads.push_back(std::thread([&execution_started, &queue, &seq, i]{
            execution_started.wait(false);
            if(auto frame = queue.get_gen_frame(); true) {
                seq.push_back(i);
                frame.gen_attr(i);
                seq.push_back(i);
            }
        }));
    }

    execution_started = true;
    execution_started.notify_all();

    for(auto& t : threads) {
        t.join();
    }

    BOOST_CHECK_EQUAL(seq.size(), 200);

    for(auto it = seq.begin(); it != seq.end(); std::advance(it, 2)) {
        BOOST_CHECK_EQUAL(*it, *std::next(it));
    }
}

BOOST_AUTO_TEST_SUITE_END()

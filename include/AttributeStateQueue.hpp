#pragma once

#include "FlatteningLifoQueue.hpp"
#include "AttributeStateFrame.hpp"
#include "utils/Protected.hpp"

template<typename T>
struct flattener<AttributeStateFrame<T>> {
    using frame_t = AttributeStateFrame<T>;
    void operator()(frame_t& top, frame_t& bottom) const {
        frame_t::merge(top, bottom);
    }
};

template <typename ATTR_GEN_T>
struct AttributeStateQueue {
    using attr_ptr_t = typename ATTR_GEN_T::attr_ptr_t;
    using attr_holder_t = typename ATTR_GEN_T::attr_holder_t;
    using attr_frame_t = AttributeStateFrame<ATTR_GEN_T>;
    using locked_frame_t = Locked<attr_frame_t>;

    using queue_t = FlatteningLifoQueue<attr_frame_t>;
    using frame_observer_t = typename queue_t::Observer;

    Protected<queue_t> queue;
    std::mutex mod_mtx;

    locked_frame_t create_mod_frame() {
        std::unique_lock lk(mod_mtx);
        return {attr_frame_t::spawn(queue.lock().get().observe().get()), std::move(lk)};
    }
    void append_mod_frame(locked_frame_t frame) {
        queue.lock().get().push(std::move(frame.get()));
    }

    struct GenFrame {
        AttributeStateQueue& parent;
        locked_frame_t frame_lkd;
        attr_ptr_t gen_attr(size_t id) {
            return frame_lkd.get().gen_attr(id);
        }
        GenFrame(AttributeStateQueue& parent, locked_frame_t frame_lkd)
            : parent(parent)
            , frame_lkd(std::move(frame_lkd)) {}
        GenFrame(GenFrame&&) = default;
        ~GenFrame() {
            parent.append_mod_frame(std::move(frame_lkd));
        }
    };
    struct ModFrame {
        AttributeStateQueue& parent;
        locked_frame_t frame_lkd;
        attr_ptr_t get_attr(size_t id) {
            return frame_lkd.get().get_attr(id);
        }
        ModFrame(AttributeStateQueue& parent, locked_frame_t frame_lkd)
            : parent(parent)
            , frame_lkd(std::move(frame_lkd)) {}
        ModFrame(ModFrame&&) = default;
        ~ModFrame() {
            parent.append_mod_frame(std::move(frame_lkd));
        }
    };
    struct ReadFrame {
        frame_observer_t frame_obs;
        ReadFrame(frame_observer_t frame_obs)
            : frame_obs(std::move(frame_obs)) {}
        ReadFrame(ReadFrame&&) = default;
        const attr_ptr_t read_attr(size_t id) const {
            return frame_obs.get().read_attr(id);
        }
    };

    AttributeStateQueue() {
        queue.lock().get().push({});
    }

    GenFrame get_gen_frame() {
        return GenFrame{*this, create_mod_frame()};
    }
    ModFrame get_mod_frame() {
        return ModFrame{*this, create_mod_frame()};
    }
    ReadFrame get_read_frame() {
        return ReadFrame{queue.lock().get().observe()};
    }
};

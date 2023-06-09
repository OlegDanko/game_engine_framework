#pragma once

#include <unordered_map>

template<typename ATTR_GEN_T>
struct AttributeStateFrame {
    using attr_holder_t = typename ATTR_GEN_T::attr_holder_t;
    using attr_ptr_t = typename ATTR_GEN_T::attr_ptr_t;

    size_t frame_id{0};
    // holds attributes created or updated during the current frame
    std::unordered_map<size_t, attr_holder_t> attr_holder_map;
    std::unordered_map<size_t, attr_ptr_t> attr_ptr_map;


    static AttributeStateFrame spawn(const AttributeStateFrame& src) {
        AttributeStateFrame frame{.frame_id = src.frame_id + 1};

        frame.attr_ptr_map = src.attr_ptr_map;

        for(auto& [id, attr] : src.attr_holder_map) {
            frame.attr_ptr_map[id] = attr.get();
        }

        return frame;
    }
    static void merge(AttributeStateFrame& dst, AttributeStateFrame& src) {
        for(auto& [id, attr] : src.attr_holder_map) {
            if(auto it = dst.attr_holder_map.find(id); dst.attr_holder_map.end() != it)
                continue;

            if(auto it = dst.attr_ptr_map.find(id); dst.attr_ptr_map.end() != it) {
                dst.attr_ptr_map.erase(it);
            }

            dst.attr_holder_map[id] = std::move(attr);
        }
    }

    attr_ptr_t gen_attr(size_t id) {
        if(attr_holder_map.find(id) != attr_holder_map.end()
            || attr_ptr_map.find(id) != attr_ptr_map.end()) {
            return nullptr;
        }

        attr_holder_map[id] = ATTR_GEN_T::gen_attribure();
        return attr_holder_map[id].get();
    }

    attr_ptr_t get_attr(size_t id) {
        if(auto it = attr_holder_map.find(id); attr_holder_map.end() != it) {
            return it->second.get();
        }

        auto it = attr_ptr_map.find(id);
        if(attr_ptr_map.end() == it) {
            return nullptr;
        }
        attr_holder_map[id] = ATTR_GEN_T::cpy_attribure(it->second);
        attr_ptr_map.erase(it);

        return attr_holder_map[id].get();
    }
    const attr_ptr_t read_attr(size_t id) const {
        if(auto it = attr_holder_map.find(id); attr_holder_map.end() != it) {
            return it->second.get();
        }
        if(auto it = attr_ptr_map.find(id); attr_ptr_map.end() != it) {
            return it->second;
        }
        return nullptr;
    }

    size_t get_id() const { return frame_id; }
};

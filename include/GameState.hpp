#pragma once

#include "AttributeStateQueue.hpp"
#include "AttributeGeneratorDefault.hpp"
#include "utils/TypeChain.hpp"

template<typename ...Ts>
struct GameState {
    template<typename T>
    using queue_t = AttributeStateQueue_DefGen<T>;
    template<typename T>
    using gen_frame_t = typename queue_t<T>::GenFrame;
    template<typename T>
    using mod_frame_t = typename queue_t<T>::ModFrame;
    template<typename T>
    using read_frame_t = typename queue_t<T>::ReadFrame;

    tmpl_type_chain<queue_t, Ts...> attribute_queues;

    template<typename ...Us>
    tmpl_type_chain<gen_frame_t, Us...> get_gen_frames() {
        return make_tmpl_type_chain<gen_frame_t, Us...>(
            get<Us>(attribute_queues).get_gen_frame()...
            );
    }
    template<typename ...Us>
    tmpl_type_chain<mod_frame_t, Us...> get_mod_frames() {
        return make_tmpl_type_chain<mod_frame_t, Us...>(
            get<Us>(attribute_queues).get_mod_frame()...
            );
    }
    template<typename ...Us>
    tmpl_type_chain<read_frame_t, Us...> get_read_frames() {
        return make_tmpl_type_chain<read_frame_t, Us...>(
            get<Us>(attribute_queues).get_read_frame()...
            );
    }
};

#pragma once

#include "AttributeStateQueue.hpp"
#include "AttributeGeneratorDefault.hpp"
#include <utils/TypeChain.hpp>

template<typename ...Ts>
struct GameState_defs {
    template<typename T>
    using queue_t = AttributeStateQueue_DefGen<T>;
    template<typename ...Us>
    using queues_tmpl = tmpl_type_chain<queue_t, Us...>;
    using queues_t = queues_tmpl<Ts...>;
    template<typename T>
    using queue_ref_t = queue_t<T>&;
    template<typename ...Us>
    using queue_refs_chain_t = tmpl_type_chain<queue_ref_t, Us...>;
    template<typename T>
    using gen_frame_t = typename queue_t<T>::GenFrame;
    template<typename T>
    using mod_frame_t = typename queue_t<T>::ModFrame;
    template<typename T>
    using read_frame_t = typename queue_t<T>::ReadFrame;

    // Generator
    struct GameStateGenerator;
    template<typename, typename>
    struct GameStateAccess;
    struct GameState;
};

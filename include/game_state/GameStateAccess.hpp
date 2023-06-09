#pragma once

#include "GameState_defs.hpp"

template<typename ...Ts>
template<typename ...Ms, typename ...Rs>
struct GameState_defs<Ts...>::GameStateAccess<types<Ms...>, types<Rs...>> {
    using mod_queues_t = queue_refs_chain_t<Ms...>;
    using read_queues_t = queue_refs_chain_t<Rs...>;
    using mod_frames_t = tmpl_type_chain<mod_frame_t, Ms...>;
    using read_frames_t = tmpl_type_chain<read_frame_t, Rs...>;

    mod_queues_t mod_queues;
    const read_queues_t read_queues;
    GameStateAccess(mod_queues_t mqs, read_queues_t rqs)
        : mod_queues(std::move(mqs))
        , read_queues(std::move(rqs)) {}

    struct Frame {
        mod_frames_t mod_frames;
        read_frames_t read_frames;
        struct GameStateObject {
            size_t id;
            Frame& frames;

            size_t get_id() {
                return id;
            }
            template<typename T>
            T* get_attr() {
                return get<T>(frames.mod_frames).get_attr(id);
            }
            template<typename T>
            const T* read_attr() const {
                return get<T>(frames.read_frames).read_attr(id);
            }
        };
        GameStateObject get_game_object(size_t id) {
            return {id, *this};
        }
    };
    Frame get_frame() {
        return Frame(
            make_tmpl_type_chain<mod_frame_t, Ms...>(
                get<Ms>(mod_queues).get_mod_frame()...),
            make_tmpl_type_chain<read_frame_t, Rs...>(
                get<Rs>(read_queues).get_read_frame()...)
            );
    }
};

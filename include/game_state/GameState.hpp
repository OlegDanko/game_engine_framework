#pragma once

#include "AttributeStateQueue.hpp"
#include "AttributeGeneratorDefault.hpp"
#include "utils/TypeChain.hpp"

template<typename ...Ts>
struct GameState {
    template<typename T>
    using queue_t = AttributeStateQueue_DefGen<T>;
    using queues_t = tmpl_type_chain<queue_t, Ts...>;
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

    queues_t attribute_queues;

    template<typename ...Us>
    queue_refs_chain_t<Us...> get_queues() {
        return make_tmpl_type_chain<queue_ref_t, Us...>(
            get<Us>(attribute_queues)...
            );
    }
    template<typename ...Us>
    queue_refs_chain_t<Us...> get_queues(types<Us...>) {
        return get_queues<Us...>();
    }

    // Generator
    struct {
        queues_t& queues;
        struct Frame {
            using gen_frames_t = tmpl_type_chain<gen_frame_t, Ts...>;
            template<typename T>
            using ptr = T*;
            gen_frames_t frames;
            template<typename ...Us>
            struct GameObject {
                size_t id;
                tmpl_type_chain<ptr, Us...> attributes;

                size_t get_id() {
                    return id;
                }
                template<typename T>
                T& get_attr() {
                    return *get<T>(attributes);
                }
            };
            template<typename ...Us>
            GameObject<Us...> gen_game_object() {
                static size_t id = 0;
                ++id;
                return GameObject<Us...>(
                    id,
                    make_tmpl_type_chain<ptr>(
                        get<Us>(frames).gen_attr(id)...)
                    );
            }
        };

        Frame get_frame() {
            return Frame(
                make_tmpl_type_chain<gen_frame_t, Ts...>(
                    get<Ts>(queues).get_gen_frame()...)
                );
        }
    } generator{attribute_queues};

    template<typename, typename>
    struct Access;
    template<typename ...Ms, typename ...Rs>
    struct Access<types<Ms...>, types<Rs...>> {
//        constexpr static bool valid = !types_intersect_v<types<Ms...>, types<Rs...>>;
        using mod_queues_t = queue_refs_chain_t<Ms...>;
        using read_queues_t = queue_refs_chain_t<Rs...>;
        using mod_frames_t = tmpl_type_chain<mod_frame_t, Ms...>;
        using read_frames_t = tmpl_type_chain<read_frame_t, Rs...>;

        mod_queues_t mod_queues;
        const read_queues_t read_queues;
        Access(mod_queues_t mqs, read_queues_t rqs)
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

    template<typename MOD_ATTRS, typename READ_ATTRS>
    Access<MOD_ATTRS, READ_ATTRS> get_access() {
        return Access<MOD_ATTRS, READ_ATTRS>(get_queues(MOD_ATTRS()),
                                             get_queues(READ_ATTRS()));
    }
};





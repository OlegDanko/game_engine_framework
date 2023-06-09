#pragma once

#include "GameState_defs.hpp"
#include <events/EventSource.hpp>
#include <events/EventTicketCourier.hpp>




template<typename ...Ts>
struct GameState_defs<Ts...>::GameStateGenerator {
    queues_t& queues;

    template<typename T>
    struct GenEventSource {
        EventSource<size_t, size_t> src; // second size_id for the frame id
    };

    template<typename ...Us>
    using gen_event_sources_tmpl = tmpl_type_chain<GenEventSource, Us...>;
    using gen_event_sources_t = gen_event_sources_tmpl<Ts...>;

    gen_event_sources_t gen_event_sources;
    EventTicketCourier gen_event_courier;

    gen_event_sources_t make_gen_event_sources() {
        return make_tmpl_type_chain<GenEventSource, Ts...>(
            GenEventSource<Ts>{make_event_source<size_t, size_t>(gen_event_courier)}...
            );
    }

    struct Frame {
        template<typename ...Us>
        using gen_frames_tmpl = tmpl_type_chain<gen_frame_t, Us...>;
        using gen_frames_t = gen_frames_tmpl<Ts...>;
        template<typename T>
        using ptr = T*;
        gen_frames_t frames;
        gen_event_sources_t& event_sources;
        EventTicketCourier& event_courier;
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

        template<typename T, typename ...Fs, typename ...Ss>
        static void notify(size_t id,
                           gen_frames_t& frames,
                           gen_event_sources_t& gen_sources) {
            get<T>(gen_sources).src.create(id, get<T>(frames).get_id());
        }

        template<typename ...Us>
        GameObject<Us...> gen_game_object() {
            static size_t id = 0;
            ++id;
            (notify<Us>(id, frames, event_sources), ...);
            return GameObject<Us...>(
                id,
                make_tmpl_type_chain<ptr>(
                    get<Us>(frames).gen_attr(id)...)
                );
        }
        ~Frame() {
            event_courier.dispatch();
        }
    };

    GameStateGenerator(queues_t& queues)
        : queues(queues)
        , gen_event_sources(make_gen_event_sources()) {}

    Frame get_frame() {
        return Frame(
            make_tmpl_type_chain<gen_frame_t, Ts...>(
                get<Ts>(queues).get_gen_frame()...),
            gen_event_sources,
            gen_event_courier
            );
    }

    template<typename T>
    EventSource<size_t>& get_gen_event_source() {
        return get<T>(gen_event_sources);
    }
};

template<typename... Us>
struct Struct {
    static void fn() {
        (doThings<Us>(), ...);
    }

private:
    template<typename U>
    static void doThings() {
        // Do U related things
    }
};

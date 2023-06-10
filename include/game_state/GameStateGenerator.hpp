#pragma once

#include "GameState_defs.hpp"
#include <events/EventSource.hpp>
#include <events/EventTicketCourier.hpp>

template<typename ...Ts>
template<typename ...Us>
struct GameState_defs<Ts...>::GameStateGenNotifier {
//struct GameStateGenNotifier {
    template<typename T>
    struct GenEventSource { EventSource<size_t, size_t> src; };
    using gen_event_sources_t = tmpl_type_chain<GenEventSource, Ts...>;
    std::unique_ptr<IEventTicketCourier> courier;
    gen_event_sources_t make_gen_event_sources() {
        return make_tmpl_type_chain<GenEventSource, Ts...>(
            GenEventSource<Ts>{make_event_source<size_t, size_t>(*courier)}...
            );
    }
    gen_event_sources_t gen_event_sources{make_gen_event_sources()};

    GameStateGenNotifier(std::unique_ptr<IEventTicketCourier> courier
                         = std::make_unique<EventTicketCourier>())
        : courier(std::move(courier)) {}

    template<typename T>
    void notify(size_t id, size_t frame) {
        get<T>(gen_event_sources).src.create(id, frame);
    }
    template<typename T>
    IEventApplier<size_t, size_t>& get_event_source() {
        return get<T>(gen_event_sources).src;

    }
    void dispatch() { courier->dispatch(); }
};

template<typename ...Ts>
template<template<typename ...> typename GEN_NOTIFIER>
struct GameState_defs<Ts...>::GameStateGenerator {
    queues_t& queues;

    using Notifier_t = GEN_NOTIFIER<Ts...>;
    Notifier_t notifier;

    struct Frame {
        using gen_frames_t = tmpl_type_chain<gen_frame_t, Ts...>;
        template<typename T>
        using ptr = T*;
        gen_frames_t frames;
        Notifier_t& notifier;

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

        size_t next_go_id() {
            static size_t id = 0;
            return ++id;
        }

        template<typename ...Us>
        GameObject<Us...> gen_game_object() {
            auto id = next_go_id();
            (notifier.template notify<Us>(id, get<Us>(frames).get_id()), ...);
            return GameObject<Us...>(
                id,
                make_tmpl_type_chain<ptr>(
                    get<Us>(frames).gen_attr(id)...)
                );
        }
        ~Frame() {
            notifier.dispatch();
        }
    };

    GameStateGenerator(queues_t& queues, Notifier_t notifier = Notifier_t())
        : queues(queues)
        , notifier(std::move(notifier)) {}

    Frame get_frame() {
        return Frame(
            make_tmpl_type_chain<gen_frame_t, Ts...>(
                get<Ts>(queues).get_gen_frame()...),
            notifier
            );
    }

    template<typename T>
    IEventApplier<size_t, size_t>& get_gen_event_source() {
        return notifier.template get_event_source<T>();
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

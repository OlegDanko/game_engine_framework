#pragma once

#include "GameState_defs.hpp"

#include <events/EventSource.hpp>
#include <events/EventTicketCourier.hpp>

template<typename ...Ts>
struct GameState_defs<Ts...>::GameStateGenNotifier {
    template<typename T>
    struct GenEventSource { EventSource<size_t, size_t> src; };
    using gen_event_sources_t = tmpl_type_chain<GenEventSource, Ts...>;
    std::unique_ptr<IEventTicketCourier> courier;
    gen_event_sources_t make_gen_event_sources() {
        return make_tmpl_type_chain<GenEventSource, Ts...>(
            GenEventSource<Ts>{*courier}...
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

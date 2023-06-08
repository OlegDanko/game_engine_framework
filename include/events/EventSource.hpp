#pragma once

#include "EventTicket.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

struct IEventTicketReceiver;

struct IEventTicketRecvRegistry {
    virtual void register_receiver(size_t src_id, IEventTicketReceiver* recv) = 0;
};

struct IEventTicketCourier : IEventTicketRecvRegistry {
    virtual void on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) = 0;
};

struct EventSourceBase {
    size_t id;
    IEventTicketCourier& courier;
    EventSourceBase(size_t id, IEventTicketCourier& c)
        : id(id)
        , courier(c) {}
};

template<typename ...Ts>
struct IEventApplier {
    virtual void apply(size_t event_id, std::function<void(Ts&...)> fn) = 0;
    virtual IEventTicketRecvRegistry& get_receiver_registry() = 0;
};

template<typename ...Ts>
struct EventSource : EventSourceBase, ITicketClosedListener, IEventApplier<Ts...> {
    std::unordered_map<size_t, std::tuple<Ts...>> events;

    EventSource(size_t id, IEventTicketCourier& c) : EventSourceBase(id, c) {}

    void create(const Ts&...args) {
        static size_t ticket_id = 0;
        ++ticket_id;
        events[ticket_id] = {args...};
        courier.on_ticket(id, std::make_shared<EventTicket>(ticket_id, *this));
    }
    void on_ticket_closed(size_t event_id) override {
        if(auto it = events.find(event_id); events.end() != it) {
            events.erase(it);
        }
    }

    void apply(size_t event_id, std::function<void(Ts&...)> fn) override {
        if(auto it = events.find(event_id); events.end() != it) {
            std::apply(fn, it->second);
        }
    }
    IEventTicketRecvRegistry& get_receiver_registry() override {
        return courier;
    }
};

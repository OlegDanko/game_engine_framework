#pragma once

#include "EventTicket.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

struct IEventTicketReceiver;

struct IEventTicketCourier {
    virtual void register_receiver(size_t src_id, IEventTicketReceiver* recv) = 0;
    virtual void on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) = 0;
    virtual ~IEventTicketCourier() = default;
};

template<typename ...Ts>
struct IEventApplier {
    virtual size_t get_id() const = 0;
    virtual void apply(IEventTicket& ticket, std::function<void(const Ts&...)> fn) = 0;
    virtual void register_receiver(IEventTicketReceiver*) = 0;
    virtual ~IEventApplier() = default;
};

template<typename ...Ts>
struct EventSource : ITicketClosedListener, IEventApplier<Ts...> {
    std::unordered_map<size_t, std::tuple<Ts...>> events;
    size_t id;
    IEventTicketCourier& courier;

    const ITicketClosedListener* as_ticket_closed_listener() const {
        return this;
    }

    EventSource(size_t id, IEventTicketCourier& c)
        : id(id)
        , courier(c) {}

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

    size_t get_id() const {
        return id;
    }

    void apply(IEventTicket& ticket, std::function<void(const Ts&...)> fn) override {
        if(ticket.get_listener_ptr() != as_ticket_closed_listener())
            return;

        if(auto it = events.find(ticket.get_id()); events.end() != it) {
            std::apply(fn, it->second);
        }
    }
    void register_receiver(IEventTicketReceiver* recv) override {
        courier.register_receiver(id, recv);
    }
};

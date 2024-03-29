#pragma once

#include "IEventApplier.hpp"
#include "IEventTicketCourier.hpp"
#include "EventTicket.hpp"

#include <unordered_map>
#include <functional>


size_t next_event_id();

template<typename ...Ts>
class EventSource : public ITicketClosedListener, public IEventApplier<Ts...> {
    std::unordered_map<size_t, std::tuple<Ts...>> events;
    size_t id;
    IEventTicketCourier& courier;

    const ITicketClosedListener* as_ticket_closed_listener() const {
        return this;
    }
public:
    EventSource(IEventTicketCourier& c)
        : id(next_event_id())
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

    size_t get_id() const override {
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

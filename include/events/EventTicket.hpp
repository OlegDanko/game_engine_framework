#pragma once

#include <cstdint>

struct ITicketClosedListener {
    virtual void on_ticket_closed(std::size_t id) = 0;
    virtual ~ITicketClosedListener() = default;
};

struct IEventTicket {
    virtual std::size_t get_id() = 0;
    virtual const ITicketClosedListener* get_listener_ptr() const = 0;
    virtual ~IEventTicket() = default;
};
struct EventTicket : IEventTicket {
    std::size_t id;
    ITicketClosedListener& l;

    EventTicket(const EventTicket&) = delete;
    EventTicket(EventTicket&&) = delete;
    void operator=(const EventTicket&) = delete;
    void operator=(EventTicket&&) = delete;

    EventTicket(std::size_t id, ITicketClosedListener& listener)
        : id(id)
        , l(listener) {}
    ~EventTicket() {
        l.on_ticket_closed(id);
    }
    std::size_t get_id() override {
        return id;
    }
    const ITicketClosedListener* get_listener_ptr() const override {
        return &l;
    }
};

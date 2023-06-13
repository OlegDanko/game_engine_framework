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

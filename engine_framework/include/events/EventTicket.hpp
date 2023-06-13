#pragma once

#include "IEventTicket.hpp"

class EventTicket : public IEventTicket {
    std::size_t id;
    ITicketClosedListener& l;

    EventTicket(const EventTicket&) = delete;
    EventTicket(EventTicket&&) = delete;
    void operator=(const EventTicket&) = delete;
    void operator=(EventTicket&&) = delete;
public:
    EventTicket(std::size_t id, ITicketClosedListener& listener);
    ~EventTicket();
    std::size_t get_id() override;
    const ITicketClosedListener* get_listener_ptr() const override;
};

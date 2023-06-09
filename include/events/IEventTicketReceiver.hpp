#pragma once

#include <memory>

struct IEventTicket;

struct IEventTicketFrame {
    virtual void add_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) = 0;
    virtual ~IEventTicketFrame() = default;
};

struct IEventTicketReceiver {
    virtual std::unique_ptr<IEventTicketFrame> get_frame() = 0;
    virtual ~IEventTicketReceiver() = default;
};

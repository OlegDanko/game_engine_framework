#pragma once

#include <memory>

struct IEventTicketReceiver;
struct IEventTicket;

struct IEventTicketCourier {
    virtual void register_receiver(size_t src_id, IEventTicketReceiver* recv) = 0;
    virtual void on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) = 0;
    virtual void dispatch() = 0;
    virtual ~IEventTicketCourier() = default;
};

#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "IEventTicketCourier.hpp"

struct IEventTicketFrame;

class EventTicketCourier : public IEventTicketCourier {
    std::unordered_set<IEventTicketReceiver*> receivers;
    std::unordered_map<size_t, std::unordered_set<IEventTicketReceiver*>> recv_by_src_id;
    std::unordered_map<IEventTicketReceiver*,
                       std::unique_ptr<IEventTicketFrame>> frames_by_recv;
public:
    void register_receiver(size_t src_id, IEventTicketReceiver* recv) override;

    void on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) override;
    void dispatch();
};

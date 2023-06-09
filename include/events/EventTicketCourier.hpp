#pragma once

#include "EventSource.hpp"
#include "IEventTicketReceiver.hpp"

#include <unordered_map>
#include <unordered_set>

struct EventTicketCourier : IEventTicketCourier {
    std::unordered_set<IEventTicketReceiver*> receivers;
    std::unordered_map<size_t, std::unordered_set<IEventTicketReceiver*>> recv_by_src_id;
    std::unordered_map<IEventTicketReceiver*,
                       std::unique_ptr<IEventTicketFrame>> frames_by_recv;

    void register_receiver(size_t src_id, IEventTicketReceiver* recv) override {
        receivers.insert(recv);
        recv_by_src_id[src_id].insert(recv);
    }

    void on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) override {
        auto it = recv_by_src_id.find(src_id);
        if(recv_by_src_id.end() == it)
            return;
        for(auto recv : it->second) {
            if(auto it = frames_by_recv.find(recv); frames_by_recv.end() != it)
                it->second->add_ticket(src_id, ticket);
        }
    }
    void dispatch() {
        typeof(frames_by_recv) new_frames;
        for(auto recv : receivers) {
            new_frames[recv] = recv->get_frame();
        }
        std::swap(new_frames, frames_by_recv);
    }
};

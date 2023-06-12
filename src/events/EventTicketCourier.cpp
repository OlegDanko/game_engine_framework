#include <events/EventTicketCourier.hpp>
#include <events/IEventTicketReceiver.hpp>

void EventTicketCourier::register_receiver(size_t src_id, IEventTicketReceiver *recv) {
    receivers.insert(recv);
    recv_by_src_id[src_id].insert(recv);
}

void EventTicketCourier::on_ticket(size_t src_id, std::shared_ptr<IEventTicket> ticket) {
    auto it = recv_by_src_id.find(src_id);
    if(recv_by_src_id.end() == it)
        return;
    for(auto recv : it->second) {
        if(auto it = frames_by_recv.find(recv); frames_by_recv.end() != it)
            it->second->add_ticket(src_id, ticket);
    }
}

void EventTicketCourier::dispatch() {
    typeof(frames_by_recv) new_frames;
    for(auto recv : receivers) {
        new_frames[recv] = recv->get_frame();
    }
    std::swap(new_frames, frames_by_recv);
}

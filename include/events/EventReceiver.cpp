#include "EventReceiver.hpp"
#include <utility>

void EventReceiver::add_tickets(ticket_queue_t tickets) {
    tickets_qq.push(std::move(tickets));
}

void EventReceiver::serve_ticket(src_ticket_pair_t &src_ticket_pair) {
    auto& [src_id, ticket] = src_ticket_pair;

    auto it = callbacks_by_src.find(src_id);
    if(callbacks_by_src.end() == it)
        return;

    for(auto cb : it->second) {
        cb(*ticket);
    }
}

void EventReceiver::serve_ticket_queue(ticket_queue_t &ticket_queue) {
    while(!ticket_queue.empty()) {
        serve_ticket(ticket_queue.front());
        ticket_queue.pop();
    }
}

void EventReceiver::serve_events() {
    auto ticket_queues = std::exchange(tickets_qq, {});
    while(!ticket_queues.empty()) {
        serve_ticket_queue(ticket_queues.front());
        ticket_queues.pop();
    }
}

std::unique_ptr<IEventTicketFrame> EventReceiver::get_frame() {
    return std::make_unique<Frame>(*this);
}

EventReceiver::Frame::Frame(EventReceiver &r) : recv(r) {}

EventReceiver::Frame::~Frame() {
    recv.add_tickets(std::move(tickets));
}

void EventReceiver::Frame::add_ticket(size_t src_id, ticket_sptr_t ticket) {
    tickets.push({src_id, std::move(ticket)});
}

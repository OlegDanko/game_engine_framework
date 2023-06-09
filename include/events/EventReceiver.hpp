#pragma once

#include "EventSource.hpp"
#include "IEventTicketReceiver.hpp"

#include <queue>
#include <unordered_map>
#include <unordered_set>

struct EventReceiver : IEventTicketReceiver {
    using ticket_sptr_t = std::shared_ptr<IEventTicket>;
    using src_ticket_pair_t = std::pair<size_t, ticket_sptr_t>;
    using ticket_queue_t = std::queue<src_ticket_pair_t>;

    std::unordered_map<size_t, std::vector<std::function<void(IEventTicket&)>>> callbacks_by_src;
    std::queue<ticket_queue_t> tickets_qq;

    void add_tickets(ticket_queue_t tickets) {
        tickets_qq.push(std::move(tickets));
    }

    struct Frame : IEventTicketFrame {
        EventReceiver& recv;
        ticket_queue_t tickets;

        Frame(EventReceiver& r) : recv(r) {}
        Frame(const Frame&) = delete;
        Frame(Frame&&) = delete;
        void operator=(const Frame&) = delete;
        void operator=(Frame&&) = delete;
        ~Frame() {
            recv.add_tickets(std::move(tickets));
        }
        void add_ticket(size_t src_id, ticket_sptr_t ticket) override {
            tickets.push({src_id, std::move(ticket)});
        }
    };

    std::unique_ptr<IEventTicketFrame> get_frame() override {
        return std::make_unique<Frame>(*this);
    }

    template<typename ...Ts>
    void register_callback(IEventApplier<Ts...>& e, std::function<void(const Ts&...)> cb) {
        e.register_receiver(this);
        callbacks_by_src[e.get_id()].push_back([&e, cb](auto& t) { e.apply(t, cb); });
    }
//    Will be used to create callbacks only for game objects with specific IDs, like player
//    template<typename ...Ts>
//    void register_id_callback(IEventApplier<size_t, Ts...>& e,
//                              std::function<void(const Ts&...)> cb) {}

    void serve_ticket(src_ticket_pair_t& src_ticket_pair) {
        auto& [src_id, ticket] = src_ticket_pair;

        auto it = callbacks_by_src.find(src_id);
        if(callbacks_by_src.end() == it)
            return;

        for(auto cb : it->second) {
            cb(*ticket);
        }
    }

    void serve_ticket_queue(ticket_queue_t& ticket_queue) {
        while(!ticket_queue.empty()) {
            serve_ticket(ticket_queue.front());
            ticket_queue.pop();
        }
    }

    void serve_events() {
        auto ticket_queues = std::exchange(tickets_qq, {});
        while(!ticket_queues.empty()) {
            serve_ticket_queue(ticket_queues.front());
            ticket_queues.pop();
        }
    }
};

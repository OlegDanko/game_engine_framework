#pragma once

#include "IEventApplier.hpp"
#include "IEventTicketReceiver.hpp"

#include <queue>
#include <unordered_map>
#include <mutex>

class EventReceiver : public IEventTicketReceiver {
    using ticket_sptr_t = std::shared_ptr<IEventTicket>;
    using src_ticket_pair_t = std::pair<size_t, ticket_sptr_t>;
    using ticket_queue_t = std::queue<src_ticket_pair_t>;
    std::string name;

    std::unordered_map<size_t, std::vector<std::function<void(IEventTicket&)>>> callbacks_by_src;
    std::queue<ticket_queue_t> tickets_qq;
    std::mutex qq_mtx;

    void add_tickets(ticket_queue_t tickets);

    struct Frame : IEventTicketFrame {
        EventReceiver& recv;
        ticket_queue_t tickets;

        Frame(EventReceiver& r);
        Frame(const Frame&) = delete;
        Frame(Frame&&) = delete;
        void operator=(const Frame&) = delete;
        void operator=(Frame&&) = delete;
        ~Frame();
        void add_ticket(size_t src_id, ticket_sptr_t ticket) override;
    };

    void serve_ticket(src_ticket_pair_t& src_ticket_pair);

    void serve_ticket_queue(ticket_queue_t& ticket_queue);

public:
    EventReceiver(const std::string& name = "") : name(name) {}
    template<typename ...Ts>
    void register_callback(IEventApplier<Ts...>& e, std::function<void(const Ts&...)> cb) {
        e.register_receiver(this);
        callbacks_by_src[e.get_id()].push_back([&e, cb](auto& t) { e.apply(t, cb); });
    }
    // TODO: Implement register_id_callback
//    Will be used to create callbacks only for game objects with specific IDs, like player
//    template<typename ...Ts>
//    void register_id_callback(IEventApplier<size_t, Ts...>& e,
//                              std::function<void(const Ts&...)> cb) {}

    void serve_events();

    std::unique_ptr<IEventTicketFrame> get_frame() override;
};

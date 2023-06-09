#pragma once

#include <functional>

struct IEventTicket;
struct IEventTicketReceiver;

template<typename ...Ts>
struct IEventApplier {
    virtual std::size_t get_id() const = 0;
    virtual void apply(IEventTicket& ticket, std::function<void(const Ts&...)> fn) = 0;
    virtual void register_receiver(IEventTicketReceiver*) = 0;
    virtual ~IEventApplier() = default;
};

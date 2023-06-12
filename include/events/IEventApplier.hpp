#pragma once

#include <functional>

struct IEventTicket;
struct IEventTicketReceiver;

template<typename ...Ts>
struct IEventApplier {
    using callback_t = std::function<void(const Ts&...)>;
    virtual std::size_t get_id() const = 0;
    virtual void apply(IEventTicket& ticket, callback_t fn) = 0;
    virtual void register_receiver(IEventTicketReceiver*) = 0;
    virtual ~IEventApplier() = default;
};

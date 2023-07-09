#include <events/EventTicket.hpp>

EventTicket::EventTicket(std::size_t id, ITicketClosedListener &listener)
    : id(id)
    , l(listener) {}

EventTicket::~EventTicket() {
    l.on_ticket_closed(id);
}

std::size_t EventTicket::get_id() {
    return id;
}

const ITicketClosedListener *EventTicket::get_listener_ptr() const {
    return &l;
}

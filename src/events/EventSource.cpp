#include <events/EventSource.hpp>

size_t next_event_id() {
    static size_t id = 0;
    return ++id;
}

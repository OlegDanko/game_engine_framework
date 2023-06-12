#pragma once

#include "GameState_defs.hpp"

#include <functional>
#include <events/EventTicketCourier.hpp>
#include <events/EventReceiver.hpp>

template<typename ...Ts>
template<typename Ms, typename Rs>
class GameState_defs<Ts...>::AttributeProcessorBase {
    using GameStateAccess_t = GameStateAccess<Ms, Rs>;
    GameStateAccess_t gs_access;
protected:
    using Base_t = AttributeProcessorBase;
    using frame_t = typename GameStateAccess_t::Frame;
    using game_object_t = typename frame_t::GameStateObject;
private:
    std::function<frame_t&()> get_frame;

    EventReceiver event_recv;
    EventTicketCourier event_courier;

    template<typename ...Us>
    using event_callback_t = std::function<void(frame_t&, const Us&...)>;

    template<typename ...Us>
    using go_event_callback_t = std::function<void(game_object_t, frame_t&, const Us&...)>;


    template<typename ...Us>
    void register_callback_(IEventApplier<Us...>& src, event_callback_t<Us...> cb) {
        std::function<void(const Us&...)> cb_ = [this, cb](const Us&...vals){
            cb(get_frame(), vals...);
        };
        event_recv.register_callback<Us...>(src, cb_);
    }

    template<typename ...Us>
    void register_object_callback_(IEventApplier<size_t, Us...>& src,
                                   go_event_callback_t<Us...> callback) {
        event_callback_t<size_t, Us...> callback_ =
            [callback](frame_t& frame, const size_t& id, const Us&...vals){
                callback(frame.get_game_object(id), frame, vals...);
            };
        register_callback_<size_t, Us...>(src, callback_);
    }
protected:
    IEventTicketCourier& get_event_courier() { return event_courier; }
    virtual void update_attributes(frame_t&) {}
    virtual void update() {}
public:
    template<typename ...Us, typename FN>
    void register_callback(IEventApplier<Us...>& src, FN callback) {
        event_callback_t<Us...> callback_ = callback;
        register_callback_(src, callback_);
    }

    template<typename ...Us, typename FN>
    void register_object_callback(IEventApplier<size_t, Us...>& src,
                                  FN callback) {
        go_event_callback_t<Us...> callback_ = callback;
        register_object_callback_(src, callback_);
    }

    AttributeProcessorBase(GameState& gs) : gs_access(gs){}
    virtual ~AttributeProcessorBase() = default;

    void update__() {
        if(auto frame = gs_access.get_frame(); true) {
            get_frame = [&]() -> auto& { return frame; };
            event_recv.serve_events();
            update_attributes(frame);
        }
        event_courier.dispatch();
        update();
    }
};

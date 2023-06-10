#pragma once

#include "GameStateGenerator.hpp"
#include "GameStateAccess.hpp"

template<typename ...Ts>
struct GameState_defs<Ts...>::GameState {
    queues_t attribute_queues;
    GameStateGenerator<GameStateGenNotifier> generator{attribute_queues};

    template<typename ...Us>
    queue_refs_chain_t<Us...> get_queues() {
        return make_tmpl_type_chain<queue_ref_t, Us...>(
            get<Us>(attribute_queues)...
            );
    }
    template<typename ...Us>
    queue_refs_chain_t<Us...> get_queues(types<Us...>) {
        return get_queues<Us...>();
    }


    template<typename MOD_ATTRS, typename READ_ATTRS>
    GameStateAccess<MOD_ATTRS, READ_ATTRS> get_access() {
        return GameStateAccess<MOD_ATTRS, READ_ATTRS>(get_queues(MOD_ATTRS()),
                                                      get_queues(READ_ATTRS()));
    }
};





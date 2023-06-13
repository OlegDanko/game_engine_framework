#include <game_state/AttributeProcessorBase.hpp>
#include <game_state/GameStateGenNotifier.hpp>
#include <game_state/GameStateGenerator.hpp>
#include <game_state/GameStateAccess.hpp>

#include <events/EventSource.hpp>
#include <events/EventReceiver.hpp>
#include <game_state/GameState.hpp>

#include <array>
#include <iostream>
#include <optional>
#include <ranges>
#include <thread>
#include <chrono>

using std::literals::chrono_literals::operator""ms;

struct Position {
    float x, y;
};

struct Speed {
    float x, y;
};

struct Hitbox {
    float x, y, x_size, y_size;

    static std::tuple<float, float> get_axis_edges(float pos, float offset, float size) {
        float x = pos + offset;
        return {x, x+size};
    }
    static std::tuple<float, float, float, float> get_edges(const Hitbox& hb,
                                                            const Position& pos) {
        return std::tuple_cat(get_axis_edges(pos.x, hb.x, hb.x_size),
                              get_axis_edges(pos.y, hb.y, hb.y_size));
    }
    static bool check_collision(const Hitbox& A_hb,
                                 const Position& A_pos,
                                 const Hitbox& B_hb,
                                 const Position& B_pos) {
        auto [ax0, ax1, ay0, ay1] = get_edges(A_hb, A_pos);
        auto [bx0, bx1, by0, by1] = get_edges(B_hb, B_pos);
        return !(ax1 < bx0 || ax0 > bx1 || ay1 < by0 || ay0 > by1);
    }
};

using GameStateDefs = GameState_defs<Position, Hitbox, Speed>;
using GameState_t = GameStateDefs::GameState;

template<typename Ms, typename Rs>
using AttributeProcessorBase = GameStateDefs::AttributeProcessorBase<Ms, Rs>;

struct ids_pair {
    size_t lo, hi;
    ids_pair(size_t a, size_t b) {
        auto more = a > b;
        lo = a * !more + b * more;
        hi = a * more + b * !more;
    }

    bool operator==(const ids_pair& that) const {
        return lo == that.lo && hi == that.hi;
    }
};

template<>
struct std::hash<ids_pair> {
    size_t operator()(const ids_pair& p) const {
        union {
            size_t s;
            std::array<uint8_t, sizeof(size_t)> arr;
        } u;
        u.s = p.lo;
        std::reverse(u.arr.begin(), u.arr.end());
        return u.s + p.hi;
    }
};

class CollisionProcessor : public AttributeProcessorBase<types<>, types<Position, Hitbox>> {
    std::vector<size_t> hb_ids;

    EventSource<size_t, size_t>
        collision_started_event{get_event_courier()},
        collision_stopped_event{get_event_courier()};

    std::unordered_map<ids_pair, bool> collision_status_map;

    void update_attributes(frame_t& frame) override {
        for(auto it_a = hb_ids.begin(); it_a != hb_ids.end(); ++it_a) {
            auto a = frame.get_game_object(*it_a);
            for(auto it_b = std::next(it_a); it_b != hb_ids.end(); ++it_b) {
                auto b = frame.get_game_object(*it_b);
                auto collision = Hitbox::check_collision(*a.template read_attr<Hitbox>(),
                                                         *a.template read_attr<Position>(),
                                                         *b.template read_attr<Hitbox>(),
                                                         *b.template read_attr<Position>());
                auto ids = ids_pair(*it_a, *it_b);
                if(collision_status_map[ids] == collision) return;
                collision_status_map[ids] = collision;

                std::string verb = collision ? " " : " don't ";
                std::cout << "Objects " << *it_a
                          << " and " << *it_b
                          << verb << "collide"
                          << std::endl;

                auto& event = collision ? collision_started_event : collision_stopped_event;

                event.create(*it_a, *it_b);
                event.create(*it_b, *it_a);
            }
        }
    }
    void update() override {}
public:
    CollisionProcessor(GameState_t& gs) : Base_t(gs) {
        Base_t::register_object_callback(
            gs.generator.template get_gen_event_source<Hitbox>(),
            [this](auto go, auto&, auto){
                std::cout << "created hitbox " << go.get_id() << std::endl;
                hb_ids.push_back(go.get_id());
            });
    }
};

template<typename ...Ts>
bool check_not_nullptr(Ts* ...vals) {
    return ((vals != nullptr) | ...);
}

class MovementProcessor : public AttributeProcessorBase<types<Position, Speed>, types<Hitbox>> {
    float edge_x0{-10.0f}, edge_x1{10.0f}, edge_y0{-10.0f}, edge_y1{10.0f};

    std::vector<size_t> speed_isd;
public:
    MovementProcessor(GameState_t& gs) : Base_t(gs) {
        register_object_callback(
            gs.generator.template get_gen_event_source<Speed>(),
            [this](auto go, auto&, auto){
                speed_isd.push_back(go.get_id());
            });
    }

    void update_attributes(frame_t& frame) override {
        for(auto id : speed_isd) {
            auto go = frame.get_game_object(id);
            auto [speed_c, pos_c, hitbox_c] = go.read_attrs<Speed, Position, Hitbox>();
            if(!check_not_nullptr(speed_c, pos_c, hitbox_c))
                continue;

            auto [x0, x1, y0, y1] = Hitbox::get_edges(*hitbox_c, *pos_c);
            auto mirror_x = (x0 < edge_x0 && speed_c->x < 0) || (x1 > edge_x1 && speed_c->x > 0);
            auto mirror_y = (y0 < edge_y0 && speed_c->y > 0) || (y1 > edge_y1 && speed_c->y < 0);

            auto speed = go.get_attr<Speed>();
            if(mirror_x) speed->x *= -1.0f;
            if(mirror_y) speed->y *= -1.0f;

            auto pos = go.get_attr<Position>();
            pos->x += speed->x;
            pos->y += speed->y;
        }
    }
};

int main()
{
    GameState_t gs;
    CollisionProcessor collision_processor(gs);
    MovementProcessor movement_processor(gs);
    gs.generator.get_frame();

    if(auto frame = gs.generator.get_frame(); true) {
        for(auto i : std::views::iota(0, 2)) {
            (void)i;
            auto go = frame.template gen_game_object<Position, Speed, Hitbox>();

            go.get_attr<Position>().x = 0.0f;
            go.get_attr<Position>().y = 0.0f;
            go.get_attr<Hitbox>().x = -0.5f;
            go.get_attr<Hitbox>().y = -0.5f;
            go.get_attr<Hitbox>().x_size = 1.0f;
            go.get_attr<Hitbox>().y_size = 1.0f;
            go.get_attr<Speed>().x = i%2 == 0 ? 0.2f : -0.2f;
        }
    }
    while(true) {
        collision_processor.update__();
        movement_processor.update__();
        std::this_thread::sleep_for(10ms);
    }

    return 0;
}

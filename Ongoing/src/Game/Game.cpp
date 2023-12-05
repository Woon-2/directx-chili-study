#include "Game/Game.hpp"

#include "Game/Box.hpp"

#include "AdditionalRanges.hpp"

Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : scene_(), renderer_(), inputSystem_(kbd, mouse, wnd.client()),
    timer_(), entities_() {
    renderer_.linkPipeline(gfx.pipeline());

    createObjects(80u, wnd, gfx, kbd, mouse);

    scene_.sortFor(renderer_);
}

void Game::update() {
    // update systems
    inputSystem_.update();

    auto elapsed = timer_.mark();

    // update entities
    std::ranges::for_each(entities_ | dereference(),
        [this, elapsed](auto& entity) {
            entity.update( elapsed );
        }
    );
}

void Game::render() {
    renderer_.render(scene_);
}

void Game::createObjects(std::size_t n, const ChiliWindow& wnd,
    Graphics& gfx, Keyboard<MyChar>& kbd, Mouse& mouse
) {
    enum {
        MAKE_BOX
    };

    auto distType = std::uniform_int_distribution<>(0, 0);

    for (auto i = decltype(n)(0); i < n; ++i)  {

        switch ( distType(rng) ) {
        case MAKE_BOX:
            createConcreteObject<Box>(wnd, gfx, kbd, mouse, Box{});
            break;

        default:
            break;
        }

    }
}

template <class T, class ... Args>
void Game::createConcreteObject( const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse, Args&& ... args
) {
    static constexpr auto pi = 3.14159f;

    auto distRadius = Distribution(15.f, 6.f);
    auto distCTP = Distribution(0.f, 2.f * pi);
    auto distDeltaCTP = Distribution(0.5f * pi, 0.2f * pi);
    auto distDeltaRTY = Distribution(0.5f * pi, 0.2f * pi);

    auto obj = std::make_unique<Entity<T>>( std::forward<Args>(args)... );

    obj->ctDrawComponent(gfx.factory(), gfx.pipeline(), scene_, wnd);
    obj->ctTransformComponent(distRadius, distCTP, distDeltaCTP, distDeltaRTY);

    obj->loader().loadAt(scene_);

    entities_.push_back( std::move(obj) );
}
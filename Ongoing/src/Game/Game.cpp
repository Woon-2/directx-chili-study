#include "Game/Game.hpp"

#include "Game/Box.hpp"

#include "AdditionalRanges.hpp"

Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : scene_(), renderer_(), inputSystem_(kbd, mouse, wnd.client()),
    timer_(), entities_() {
    renderer_.linkPipeline(gfx.pipeline());

    auto box = std::make_unique<Entity<Box>>( Box{} );
    box->ctDrawComponent(gfx.factory(), gfx.pipeline(), scene_, wnd);
    box->ctInputComponent( inputSystem_.mousePointConverter(),
        AppMousePoint{ .x=0.f, .y=0.f }
    );
    box->ctTransformComponent();

    box->loader().loadAt(scene_);
    box->loader().loadAt(inputSystem_);

    entities_.push_back( std::move(box) );

    scene_.sortFor(renderer_);
}

void Game::update() {
    // update systems
    inputSystem_.update();

    // update entities
    std::ranges::for_each(entities_ | dereference(),
        [this](auto& entity) {
            entity.update( timer_.peek() );
        }
    );
}

void Game::render() {
    renderer_.render(scene_);
}
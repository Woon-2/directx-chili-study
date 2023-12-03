#include "Game/Game.hpp"

#include "Game/Box.hpp"

Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : scene_(), renderer_(), inputSystem_(kbd, mouse), entities_() {
    renderer_.linkPipeline(gfx.pipeline());

    auto box = std::make_unique<Entity<Box>>( Box{} );
    box->ctDrawComponent(gfx.factory(), scene_, wnd);
    box->loader().loadAt(scene_);

    entities_.push_back( std::move(box) );

    scene_.sortFor(renderer_);
}

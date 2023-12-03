#ifndef __Game
#define __Game

#include "Scene.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "Entity.hpp"
#include "Box.hpp"

#include <memory>

#include "ChiliWindow.hpp"
#include "Graphics.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

class Game {
public:
    using MyChar = ChiliWindow::MyChar;

    Game(const ChiliWindow& wnd, Graphics<ChiliWindow>& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse
    ) : scene_(), renderer_(), inputSystem_(kbd, mouse), entities_() {
        renderer_.linkPipeline(gfx.pipeline());

        auto box = std::make_unique<Entity<Box>>( Box{} );
        box->ctDrawComponent(gfx.factory(), scene_);
        box->loader().loadAt(scene_);

        entities_.push_back( std::move(box) );

        scene_.sortFor(renderer_);
    }

    void update() {

    }

    void render() {
        renderer_.render(scene_);
    }

private:
    Scene scene_;
    Renderer renderer_;
    InputSystem<MyChar> inputSystem_;
    std::vector< std::unique_ptr<IEntity> > entities_;
};

#endif  // __Game
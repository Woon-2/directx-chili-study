#ifndef __Game
#define __Game

#include "Scene.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "Entity.hpp"

#include "App/ChiliWindow.hpp"
#include "App/Keyboard.hpp"
#include "App/Mouse.hpp"
#include "GFX/Core/Graphics.hpp"

#include <memory>

class Game {
public:
    using MyChar = ChiliWindow::MyChar;

    Game(const ChiliWindow& wnd, Graphics& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse
    );

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
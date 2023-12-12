#ifndef __Game
#define __Game

#include "RendererSystem.hpp"
#include "InputSystem.hpp"
#include "Entity.hpp"
#include "Timer.hpp"

#include "App/ChiliWindow.hpp"
#include "App/Keyboard.hpp"
#include "App/Mouse.hpp"
#include "GFX/Core/Graphics.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include <memory>

class Game {
public:
    using MyTimer = Timer<float>;
    using MyChar = ChiliWindow::MyChar;

    Game(const ChiliWindow& wnd, Graphics& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse
    );

    void update();

    void render();

private:
    void createObjects(std::size_t n, const ChiliWindow& wnd,
        Graphics& gfx, Keyboard<MyChar>& kbd, Mouse& mouse
    );
    template <class T, class ... Args>
    void createConcreteObject( const ChiliWindow& wnd, Graphics& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse, Args&& ... args
    );

    RendererSystem rendererSystem_;
    GFXStorage rendererStorage_;
    InputSystem<MyChar> inputSystem_;
    MyTimer timer_;
    std::vector< std::unique_ptr<IEntity> > entities_;
};

#endif  // __Game
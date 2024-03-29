#ifndef __Game
#define __Game

#include "GFX/Scenery/RendererSystem.hpp"
#include "InputSystem.hpp"
#include "Entity.hpp"
#include "Timer.hpp"
#include "GFX/Scenery/Camera.hpp"
#include "CameraControl.hpp"
#include "CoordSystem.hpp"

#include "App/ChiliWindow.hpp"
#include "App/Keyboard.hpp"
#include "App/Mouse.hpp"
#include "GFX/Core/Graphics.hpp"

// include for distributions temporary.
#include "GTransformComponent.hpp"

// include for controlling point light temporary.
#include "GFX/Scenery/PointLight.hpp"
#include "PointLightControl.hpp"

#include "InputComponent.hpp"
#include "SimulationUI.hpp"

#include <memory>

class Game;

template<>
class KeyboardInputComponent<Game, CHAR> : public IKeyboardInputComponent<CHAR> {
public:
    KeyboardInputComponent()
        : bSimulate_(true) {}

    void receive(const Keyboard<MyChar>::Event& ev) override {
        if (ev.pressed() && ev.keycode() == VK_SPACE) {
            bSimulate_ = false;
        }
        else if (ev.released() && ev.keycode() == VK_SPACE) {
            bSimulate_ = true;
        }
    }

    bool willSimulate() const noexcept {
        return bSimulate_;
    }

private:
    bool bSimulate_;
};

class Game {
public:
    using MyTimer = Timer<float>;
    using MyChar = ChiliWindow::MyChar;
    using MyIC = KeyboardInputComponent<Game, CHAR>;

    Game(const ChiliWindow& wnd, gfx::Graphics& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse
    );
    ~Game();

    void update();
    void render();

private:
    void updateEntities(milliseconds elapsed);

    void createObjects(std::size_t n, const ChiliWindow& wnd,
        gfx::Graphics& gfx, Keyboard<MyChar>& kbd, Mouse& mouse
    );
    template <class T, class ... Args>
    void createConcreteObject( Distribution distRadius,
        Distribution distCTP, Distribution distDeltaCTP,
        Distribution distDelteRTY, std::uniform_int_distribution<> distScene,
        const ChiliWindow& wnd, gfx::Graphics& gfx, Keyboard<MyChar>& kbd,
        Mouse& mouse, Args&& ... args
    );

    void createIlBox( Distribution distRadius,
        Distribution distCTP, Distribution distDeltaCTP,
        Distribution distDelteRTY, const ChiliWindow& wnd, gfx::Graphics& gfx,
        Keyboard<MyChar>& kbd, Mouse& mouse
    );

    gfx::scenery::RendererSystem rendererSystem_;
    InputSystem<MyChar> inputSystem_;
    CoordSystem coordSystem_;
    MyTimer timer_;
    gfx::scenery::Camera camera_;
    CameraControl cameraControl_;
    std::vector< std::unique_ptr<IEntity> > entities_;
    gfx::scenery::LightEntity light_;
    PointLightControl pointLightControl_;
    SimulationUI simulationUI_;

    std::shared_ptr<MyIC> ic_;
};

#endif  // __Game
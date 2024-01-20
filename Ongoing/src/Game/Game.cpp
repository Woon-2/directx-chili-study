#include "Game/Game.hpp"

#include "Game/IlluminatedBox.hpp"

#include "Game/GFXCMDLogger.hpp"
#include "Game/GFXCMDLogGUIView.hpp"

#include "AdditionalRanges.hpp"
#include "GFX/Core/GraphicsNamespaces.hpp"

#include "Image/GDIPlusMgr.hpp"
// do as chili do, it's temporary.
GDIPlusManager gdipm;


Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : rendererSystem_( gfx.factory(), gfx.pipeline() ),
    inputSystem_( kbd, mouse, wnd.client() ),
    coordSystem_(), timer_(), camera_(),
    cameraControl_(), entities_(), light_(),
    pointLightControl_(),
    simulationUI_(), ic_( std::make_shared<MyIC>() ) {

    camera_.setParams(dx::XM_PIDIV2, 1.f, 0.5f, 40.f);
    camera_.attach(coordSystem_);
    camera_.coordSystem().adjustGlobal(
        dx::XMMatrixTranslation(0.f, 0.f, -20.f)
    );
    cameraControl_.show();

    auto slotBPhongRenderer = rendererSystem_
        .addRenderer<BPhongRenderer>();
    rendererSystem_.enableLog(slotBPhongRenderer);
    rendererSystem_.sync(slotBPhongRenderer);
    rendererSystem_.scene(slotBPhongRenderer).setVision( camera_.vision() );
    rendererSystem_.scene(slotBPhongRenderer).addLayer();

    auto slotSolidRenderer = rendererSystem_
        .addRenderer<SolidRenderer>();
    rendererSystem_.enableLog(slotSolidRenderer);
    rendererSystem_.sync(slotSolidRenderer);
    rendererSystem_.scene(slotSolidRenderer).setVision( camera_.vision() );

    light_.ctLuminance(gfx.factory(), rendererSystem_.storage());
    light_.luminance().loader().loadAt(
        rendererSystem_.adapt<LSceneAdapter>(slotBPhongRenderer)
    );
    light_.luminance().loader().loadAt(coordSystem_);
    light_.luminance().sync(rendererSystem_.renderer(slotBPhongRenderer));
    light_.ctViz(gfx.factory(), rendererSystem_.storage(), wnd.client());
    light_.viz().loader().loadAt(rendererSystem_.scene(slotSolidRenderer).layer(0));

    inputSystem_.setListner(ic_);

    coordSystem_.traverse();

    createObjects(80u, wnd, gfx, kbd, mouse);
}

Game::~Game() {
    coordSystem_.destroyCascade();
}

void Game::update() {
    auto elapsed = timer_.mark();
    // update systems
    inputSystem_.update();
    pointLightControl_.submit(light_);
    cameraControl_.reflect(camera_.coordSystem());

    // coord system may be affected by other systems,
    // so update coord system lastly.
    coordSystem_.traverse();

    // update camera vision via updated coordinate systems.
    // (it doesn't modifies other cooridnate systems.)
    camera_.update();

    updateEntities(elapsed);

    // coord systems may be changed during updating entities,
    // so update coord system once more.
    coordSystem_.traverse();
}

void Game::render() {
    rendererSystem_.render();
    simulationUI_.render();
    cameraControl_.render();
    pointLightControl_.render();
    GFXCMDLOG.advance();
    GFXCMDLOG_GUIVIEW.render();
}

void Game::updateEntities(milliseconds elapsed) {
    if (ic_->willSimulate()) {
        light_.update(elapsed);

        std::ranges::for_each(entities_ | dereference(),
            [this, elapsed](auto& entity) {
                entity.update( elapsed * simulationUI_.speedFactor() );
            }
        );
    }
}

void Game::createObjects(std::size_t n, const ChiliWindow& wnd,
    Graphics& gfx, Keyboard<MyChar>& kbd, Mouse& mouse
) {
    while (n--) {
        createIlBox( Distribution(15.f, 6.f), /* distRadius */   \
            Distribution(0.f, dx::XM_2PI), /* distCTP */    \
            Distribution(0.03f * dx::XM_PI, 0.01f * dx::XM_PI), /* distDeltaCTP */    \
            Distribution(dx::XM_PIDIV2, 0.2f * dx::XM_PI) /* distDeltaRTY */,
            wnd, gfx, kbd, mouse
        );
    }
}

void Game::createIlBox( Distribution distRadius,
    Distribution distCTP,  Distribution distDeltaCTP,
    Distribution distDeltaRTY, const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) {
    auto obj = std::make_unique<Entity<IlluminatedBox>>();

    obj->ctDrawComponent( gfx.factory(), gfx.pipeline(),
        rendererSystem_.storage(), wnd
    );
    obj->ctTransformComponent(distRadius, distCTP, distDeltaCTP, distDeltaRTY);

    obj->loader().loadAt( rendererSystem_.adapt<LSceneAdapter>(0u) );

    entities_.push_back( std::move(obj) );
}
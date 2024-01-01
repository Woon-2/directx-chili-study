#include "Game/Game.hpp"

#include "Game/Box.hpp"
#include "Game/Cone.hpp"
#include "Game/Plane.hpp"
#include "Game/Prism.hpp"
#include "Game/Sphere.hpp"
#include "Game/Sheet.hpp"
#include "Game/SkinnedBox.hpp"

#include "Game/GFXCMDLogger.hpp"
#include "Game/GFXCMDLogGUIView.hpp"

#include "AdditionalRanges.hpp"

#include "Image/GDIPlusMgr.hpp"
// do as chili do, it's temporary.
GDIPlusManager gdipm;


Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : rendererSystem_( gfx.factory(), gfx.pipeline() ),
    inputSystem_( kbd, mouse, wnd.client() ),
    coordSystem_( std::make_shared<CoordSystem>() ),
    timer_(), camera_(), entities_(),
    simulationUI_(), ic_( std::make_shared<MyIC>() ) {

    camera_.setParams(dx::XM_PIDIV2, 1.f, 0.5f, 40.f);
    camera_.attach(coordSystem_);
    coordSystem_->addChild(camera_.coordSystem());
    camera_.coordSystem()->adjustGlobal(
        dx::XMMatrixTranslation(0.f, 0.f, -20.f)
    );

    auto slotIndexedRender = rendererSystem_
        .addRenderer<IndexedRenderer>();
    rendererSystem_.enableLog(slotIndexedRender);
    rendererSystem_.sync(slotIndexedRender);
    rendererSystem_.scene(slotIndexedRender).setVision( camera_.vision() );

    auto slotBlendedRender = rendererSystem_
        .addRenderer<BlendedRenderer>();
    rendererSystem_.enableLog(slotBlendedRender);
    rendererSystem_.sync(slotBlendedRender);
    rendererSystem_.scene(slotBlendedRender).setVision( camera_.vision() );

    auto slotTexturedRender = rendererSystem_
        .addRenderer<TexturedRenderer>();
    rendererSystem_.enableLog(slotTexturedRender);
    rendererSystem_.sync(slotTexturedRender);
    rendererSystem_.scene(slotTexturedRender).setVision( camera_.vision() );

    inputSystem_.setListner(ic_);

    createObjects(80u, wnd, gfx, kbd, mouse);
}

void Game::update() {
    auto elapsed = timer_.mark();
    // update systems
    inputSystem_.update();
    camera_.update();

    // coord system may be affected by other systems,
    // so update coord system lastly.
    coordSystem_->traverse();

    // update entities
    if (ic_->willSimulate()) {
        std::ranges::for_each(entities_ | dereference(),
            [this, elapsed](auto& entity) {
                entity.update( elapsed * simulationUI_.speedFactor() );
            }
        );
    }

    // coord systems may be changed during updating entities,
    // so update coord system once more.
    coordSystem_->traverse();
}

void Game::render() {
    rendererSystem_.render();
    simulationUI_.render();
    GFXCMDLOG.advance();
    GFXCMDLOG_GUIVIEW.render();
}

void Game::createObjects(std::size_t n, const ChiliWindow& wnd,
    Graphics& gfx, Keyboard<MyChar>& kbd, Mouse& mouse
) {
    enum {
        MAKE_BOX,
        MAKE_CONE,
        MAKE_PLANE,
        MAKE_PRISM,
        MAKE_SPHERE,
        MAKE_SHEET,
        MAKE_SKINNED_BOX,
        SIZE
    };

    auto distType = std::uniform_int_distribution<>(MAKE_SKINNED_BOX, MAKE_SKINNED_BOX);
    auto distTesselation = std::uniform_int_distribution<std::size_t>(3u, 48u);

    for (auto i = decltype(n)(0); i < n; ++i)  {
        static constexpr auto pi = 3.14159f;

    #define BASIC_DISTS \
        Distribution(15.f, 6.f), /* distRadius */   \
        Distribution(0.f, 2.f * pi), /* distCTP */    \
        Distribution(0.03f * pi, 0.01f * pi), /* distDeltaCTP */    \
        Distribution(0.5f * pi, 0.2f * pi) /* distDeltaRTY */

        switch ( distType(rng) ) {
        case MAKE_BOX:
            createConcreteObject<Box>( BASIC_DISTS,
                std::uniform_int_distribution<>(0, 1),
                wnd, gfx, kbd, mouse
            );
            break;

        case MAKE_CONE:
            createConcreteObject<Cone>( BASIC_DISTS,
                std::uniform_int_distribution<>(0, 1),
                wnd, gfx, kbd, mouse,
                distTesselation(rng)
            );
            break;

        case MAKE_PLANE:
            createConcreteObject<Plane>( BASIC_DISTS,
                std::uniform_int_distribution<>(0, 1),
                wnd, gfx, kbd, mouse,
                distTesselation(rng), distTesselation(rng)
            );
            break;

        case MAKE_PRISM:
            createConcreteObject<Prism>( BASIC_DISTS,
                std::uniform_int_distribution<>(0, 1),
                wnd, gfx, kbd, mouse,
                distTesselation(rng)
            );
            break;

        case MAKE_SPHERE:
            createConcreteObject<Sphere>( BASIC_DISTS,
                std::uniform_int_distribution<>(0, 1),
                wnd, gfx, kbd, mouse,
                distTesselation(rng), distTesselation(rng)
            );
            break;

        case MAKE_SHEET:
            createConcreteObject<Sheet>( BASIC_DISTS,
                std::uniform_int_distribution<>(2, 2),
                wnd, gfx, kbd, mouse
            );
            break;

        case MAKE_SKINNED_BOX:
            createConcreteObject<SkinnedBox>( BASIC_DISTS,
                std::uniform_int_distribution<>(2, 2),
                wnd, gfx, kbd, mouse
            );
            break;

        default:
            break;
        }

    }
}

template <class T, class ... TesselationFactors>
void Game::createConcreteObject( Distribution distRadius,
    Distribution distCTP,  Distribution distDeltaCTP,
    Distribution distDeltaRTY, std::uniform_int_distribution<> distScene,
    const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse,
    TesselationFactors&& ... tesselationFactors
) {
    auto obj = std::make_unique<Entity<T>>();

    obj->ctDrawComponent( gfx.factory(), gfx.pipeline(),
        rendererSystem_.storage(), wnd,
        std::forward<TesselationFactors>(tesselationFactors)...
    );
    obj->ctTransformComponent(distRadius, distCTP, distDeltaCTP, distDeltaRTY);

    obj->loader().loadAt(rendererSystem_.scene( distScene(rng) ));

    entities_.push_back( std::move(obj) );
}
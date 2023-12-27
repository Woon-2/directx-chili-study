#include "Game/Game.hpp"

#include "Game/Box.hpp"
#include "Game/Cone.hpp"
#include "Game/Plane.hpp"
#include "Game/Prism.hpp"
#include "Game/Sphere.hpp"
#include "Game/Sheet.hpp"

#include "Game/GFXCMDLogger.hpp"
#include "Game/GFXCMDLogFileView.hpp"

#include "AdditionalRanges.hpp"

#include "Image/GDIPlusMgr.hpp"
// do as chili do, it's temporary.
GDIPlusManager gdipm;

Game::Game(const ChiliWindow& wnd, Graphics& gfx,
    Keyboard<MyChar>& kbd, Mouse& mouse
) : rendererSystem_( gfx.factory(), gfx.pipeline() ),
    inputSystem_( kbd, mouse, wnd.client() ),
    timer_(), entities_() {
    auto slotIndexedRender = rendererSystem_
        .addRenderer<IndexedRenderer>();
    rendererSystem_.enableLog(slotIndexedRender);
    rendererSystem_.sync(slotIndexedRender);

    auto slotBlendedRender = rendererSystem_
        .addRenderer<BlendedRenderer>();
    rendererSystem_.enableLog(slotBlendedRender);
    rendererSystem_.sync(slotBlendedRender);

    auto slotTexturedRender = rendererSystem_
        .addRenderer<TexturedRenderer>();
    rendererSystem_.enableLog(slotTexturedRender);
    rendererSystem_.sync(slotTexturedRender);

    createObjects(80u, wnd, gfx, kbd, mouse);
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
    rendererSystem_.render();
    GFXCMDLOG.advance();
    GFXCMDLOG_FILEVIEW.report();
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
        SIZE
    };

    auto distType = std::uniform_int_distribution<>(0, SIZE - 1);
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
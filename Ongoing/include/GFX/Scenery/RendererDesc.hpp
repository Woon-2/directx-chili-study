#ifndef __RendererDesc
#define __RendererDesc

#include "GFX/Core/Storage.hpp"

#include <typeindex>
#include <tuple>
#include <vector>

namespace gfx {
namespace scenery {

struct RendererDesc {
    struct Header {
        GFXStorage::ID IDVertexShader;
        GFXStorage::ID IDPixelShader;
        std::type_index IDType;
    };

    auto reflect() const noexcept {
        return std::tie(
            header.IDVertexShader,
            header.IDPixelShader,
            header.IDType
        );
    }

    auto operator<=>(const RendererDesc& rhs) const noexcept {
        return reflect() <=> rhs.reflect();
    }

    Header header;
    std::vector<GFXStorage::ID> IDs;
};

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __RendererDesc
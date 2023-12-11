#ifndef __RendererDesc
#define __RendererDesc

#include "GFX/Core/GraphicsStorage.hpp"

#include <typeindex>
#include <tuple>
#include <unordered_set>

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
    std::unordered_set<GFXStorage::ID> IDs;
};

#endif  // __RendererDesc
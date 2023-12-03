#ifndef __RenderDesc
#define __RenderDesc

#include "GFX/Core/GraphicsStorage.hpp"

#include <typeindex>
#include <tuple>
#include <unordered_set>

struct RenderDesc {
    struct Header {
        std::type_index typeID;
        GFXStorage::ID vertexShaderID;
        GFXStorage::ID pixelShaderID;
    };

    auto reflect() const noexcept {
        return std::tie(
            header.vertexShaderID,
            header.pixelShaderID,
            header.typeID
        );
    }

    auto operator<=>(const RenderDesc& rhs) const noexcept {
        return reflect() <=> rhs.reflect();
    }

    Header header;
    std::unordered_set<GFXStorage::ID> IDs;
};

#endif  // __RenderDesc
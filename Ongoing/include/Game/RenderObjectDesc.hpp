#ifndef __RenderObjectDesc
#define __RenderObjectDesc

#include "GFX/Core/GraphicsStorage.hpp"

#include <typeindex>
#include <tuple>
#include <unordered_set>

struct RenderObjectDesc {
    struct Header {
        GFXStorage::ID IDBuffer;
        std::type_index IDType;
    };

    auto reflect() const noexcept {
        return std::tie(
            header.IDBuffer,
            header.IDType
        );
    }

    auto operator<=>(const RenderObjectDesc& rhs) const noexcept {
        return reflect() <=> rhs.reflect();
    }

    Header header;
    std::unordered_set<GFXStorage::ID> IDs;
};

#endif  // __RenderObjectDesc
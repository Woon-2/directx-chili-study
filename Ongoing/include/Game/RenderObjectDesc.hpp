#ifndef __RenderObjectDesc
#define __RenderObjectDesc

#include "GFX/Core/Storage.hpp"

#include <typeindex>
#include <tuple>
#include <vector>

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
    std::vector<GFXStorage::ID> IDs;
};

#endif  // __RenderObjectDesc
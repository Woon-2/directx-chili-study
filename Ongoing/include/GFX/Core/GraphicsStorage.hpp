#ifndef __GraphicsStorage
#define __GraphicsStorage

#include "GFX/PipelineObjects/Bindable.hpp"

#include <map>
#include <memory>
#include <typeindex>
#include <optional>
#include <cassert>

#include "LRUCache.hpp"

class GFXStorage {
public:
    using ID = int;

    GFXStorage()
        : IDCache_(IDCACHE_CACHELINE_SIZE, IDCACHE_NUM_CACHELINE),
        resources_() {}

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::load lead to memory leak")]]
    const ID load(Args&& ... args) {
        auto tmp = std::shared_ptr<T>( new T(std::forward<Args>(args)...) );
        auto id = makeID();
        resources_.try_emplace(id, std::move(tmp));

        return id;
    }

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::cache lead to memory leak")]]
    const ID cache(Args&& ... args) {
        if ( auto cache_res = IDCache_.at( std::type_index( typeid(T) ) ); cache_res.hit() ) {
            return cache_res.value();
        }

        const auto id = load<T>( std::forward<Args>(args)... );
        IDCache_.try_emplace( std::type_index( typeid(T) ), id );

        return id;
    }

    std::optional<IBindable*> get(const ID& id) const noexcept {
        if ( auto it = resources_.find(id); it != resources_.end() ) {
            return (it->second).get();
        }
        return std::nullopt;
    }

    template <class T>
    std::optional<IBindable*> get() const noexcept {
        if ( auto cache_res = IDCache_.at( typeid(T) ); cache_res.hit() ) {
            assert( resources_.contains( cache_res.value() ) );
            return resources_.at( cache_res.value() ).get();
        }
        return std::nullopt;
    }

private:
    static constexpr std::size_t IDCACHE_CACHELINE_SIZE = 0x08u;
    static constexpr std::size_t IDCACHE_NUM_CACHELINE = 0x40u;
    static int IDDistribution;

    static const ID makeID() noexcept {
        // maybe occur overflow.
        // subsitute this with robust algorithm later.
        return IDDistribution++;
    }
    
    LRUCache< std::type_index, ID > IDCache_;
    std::map< ID, std::shared_ptr<IBindable> > resources_;
};

int GFXStorage::IDDistribution = 0;

#endif  // __GraphicsStorage
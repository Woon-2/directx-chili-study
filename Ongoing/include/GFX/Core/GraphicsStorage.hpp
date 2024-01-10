#ifndef __GraphicsStorage
#define __GraphicsStorage

#include "GFX/PipelineObjects/Bindable.hpp"

#include <map>
#include <memory>
#include <typeindex>
#include <optional>
#include <cassert>
#include <ranges>
#include <algorithm>

#include "LRUCache.hpp"

class GFXStorage {
public:
    using ID = int;

    struct Pair {
        ID id;
        IBindable* bindee;  
    };

    GFXStorage()
        : IDCache_(IDCACHE_CACHELINE_SIZE, IDCACHE_NUM_CACHELINE),
        resources_() {}

    ~GFXStorage() {
        std::ranges::for_each( resources_, [](auto& idResPair) {
            delete idResPair.second;
        } );
    }

    GFXStorage(const GFXStorage&) = default;
    GFXStorage& operator=(const GFXStorage&) noexcept = default;
    GFXStorage(GFXStorage&&) = default;
    GFXStorage& operator=(GFXStorage&&) noexcept = default;

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::load lead to memory leak")]]
    const Pair load(Args&& ... args) {
        auto bindee = new T(std::forward<Args>(args)...);
        auto id = makeID();
        resources_.try_emplace(id, bindee);

        return Pair{id, bindee};
    }

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::cache lead to memory leak")]]
    const Pair cache(Args&& ... args) {
        if ( auto cache_res = IDCache_.at( std::type_index( typeid(T) ) ); cache_res.hit() ) {
            const auto& id = cache_res.value();
            const auto& bindee = resources_.at(id);
            return Pair{id, bindee};
        }

        const auto ret = load<T>( std::forward<Args>(args)... );
        IDCache_.try_emplace( std::type_index( typeid(T) ), ret.id );

        return ret;
    }

    std::optional<IBindable*> get(const ID& id) const noexcept {
        if ( auto it = resources_.find(id); it != resources_.end() ) {
            return (it->second);
        }
        return std::nullopt;
    }

    template <class T>
    std::optional<IBindable*> get() const noexcept {
        return get( typeid(T) );
    }

    std::optional<IBindable*> get(std::type_index tid) const noexcept {
        if ( auto cache_res = IDCache_.at( tid ); cache_res.hit() ) {
            assert( resources_.contains( cache_res.value() ) );
            return resources_.at( cache_res.value() );
        }
        return std::nullopt;
    }

    bool search(const ID& id) const noexcept {
        return resources_.contains(id);
    }

    template <class T>
    bool search() const noexcept {
        return search( typeid(T) );
    }

    bool search(std::type_index tid) const noexcept {
        return IDCache_.contains(tid);
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
    std::map< ID, IBindable* > resources_;
};

#include "GraphicsException.hpp"

#include <functional>
#include <tuple>
#include <type_traits>

class GFXMappedResource {
public:
    template <class T>
    struct Type {};

    GFXMappedResource() = default;

    template <class T, class ... Args>
    GFXMappedResource( Type<T>, GFXStorage& storage, Args&&... args )
        : ctor_( makeConfiguration<T, Args...>(
            std::forward_as_tuple( std::forward<Args>(args)... ), false
        ) ),
        cacheKey_(), resPair_( ctor_(storage) ),
        pStorage_(&storage) {}

    template <class T, class ... Args>
    GFXMappedResource( Type<T>, std::type_index tid, GFXStorage& storage,
        Args&&... args
    ) : ctor_( makeConfiguration<T, Args...>(
            std::forward_as_tuple( std::forward<Args>(args)... ), true
        ) ),
        cacheKey_(tid), resPair_( ctor_(storage) ),
        pStorage_(&storage) {}

    void sync(GFXStorage& storage) {
        unload();
        pStorage_ = &storage;
    }

    bool valid() const noexcept {
        if ( cacheKey_.has_value() ) {
            return searchWithType();
        }
        else {
            return searchWithID();
        }
    }

    template <class T, class ... Args>
    void config(Args&& ... args) {
        ctor_ = makeConfiguration<T, Args...>(
            std::forward_as_tuple(std::forward<Args>(args)...)
        );
    }

    template <class T, class ... Args>
    void config(Type<T>, Args&& ... args) {
        ctor_ = makeConfiguration<T, Args...>(
            std::forward_as_tuple(std::forward<Args>(args)...)
        );
    }

    IBindable* get() noexcept {
        return resPair_.bindee;
    }

    const IBindable* get() const noexcept {
        return resPair_.bindee;
    }

    template <class T>
    T& as() noexcept {
        return *static_cast<T*>(resPair_.bindee);
    }
    
    template <class T>
    const T& as() const noexcept {
        return *static_cast<const T*>(resPair_.bindee);
    }

    IBindable* operator->() noexcept {
        return get();
    }

    const IBindable* operator->() const noexcept {
        return get();
    }

    const GFXStorage::ID id() const noexcept {
        return resPair_.id;
    }

    void remap() {
        unload();
        resPair_ = ctor_(*pStorage_);
    }

    void unload() {
        // remove this from mapped storage.
        // for now, we doesn't need to change the storage.
        // implement it later
    }

private:
    bool searchWithID() const noexcept {
        return pStorage_->search( resPair_.id );
    }

    bool searchWithType() const noexcept {
        return pStorage_->search( cacheKey_.value() );
    }

    template <class T>
    static auto instantiateArg(T&& arg) {
        if constexpr (std::is_lvalue_reference_v<T>) {
            return std::ref(arg);
        }
        else {
            return std::move(arg);
        }
    }

    template <class ... Args>
    static auto instantiateArgs(Args&& ... args) {
        return std::make_tuple( instantiateArg(std::forward<Args>(args))... );
    }

    template <class T, class ... Args>
    static const GFXStorage::Pair ctorImplLoad(
        GFXStorage& storage, Args&& ... args
    ) {
        return storage.load<T>( std::forward<Args>(args)... );
    }

    template <class T, class ... Args>
    static const GFXStorage::Pair ctorImplCache(
        GFXStorage& storage, Args&& ... args
    ) {
        return storage.cache<T>( std::forward<Args>(args)... );
    }

    template <class T, class ... Args>
    static std::function<GFXStorage::Pair(GFXStorage&)>
    makeConfiguration( std::tuple<Args&&...>&& args,
        bool willCache = false
    ) {
        if (willCache) {
            return makeCachedConfiguration<T, Args...>(std::move(args));
        }
        else {
            return makeLoadConfiguration<T, Args...>(std::move(args));
        }
    }

    template <class T, class ... Args>
    static auto makeCachedConfiguration(std::tuple<Args&&...>&& args) {
        return [ args = std::apply(instantiateArgs<Args...>, std::move(args)) ]
            (GFXStorage& strg) -> const GFXStorage::Pair {
            return std::apply( ctorImplCache<T, Args...>,
                std::tuple_cat( std::tie(strg), args )
            );
        };
    }

    template <class T, class ... Args>
    static auto makeLoadConfiguration(std::tuple<Args&&...>&& args) {
        return [ args = std::apply(instantiateArgs<Args...>, std::move(args)) ]
            (GFXStorage& strg) -> const GFXStorage::Pair {
            return std::apply( ctorImplLoad<T, Args...>,
                std::tuple_cat( std::tie(strg), args )
            );
        };
    }

    std::function<GFXStorage::Pair(GFXStorage&)> ctor_;
    std::optional<std::type_index> cacheKey_;
    GFXStorage::Pair resPair_;
    GFXStorage* pStorage_;
};

#endif  // __GraphicsStorage
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
#include <coroutine>
#include <exception>
#include <variant>
#include <optional>
#include <functional>
#include <type_traits>
#include <utility>
#include <concepts>

#include "LRUCache.hpp"

class GFXRes;

namespace detail {

template <class TRes>
class GFXResViewBase {
public:
    GFXResViewBase()
        : res_(nullptr) {}

    GFXResViewBase(TRes& res)
        : res_(&res) {}

    GFXResViewBase(const GFXResViewBase& other) = default;
    GFXResViewBase& operator=(const GFXResViewBase& other) = default;

    GFXResViewBase(GFXResViewBase&& other) noexcept
        : res_(std::exchange(other.res_, nullptr)) {}

    GFXResViewBase& operator=(GFXResViewBase&& other) noexcept {
        res_ = std::exchange(other.res_, nullptr);
        return *this;
    }

    TRes& operator*() const {
        assert(res_ != nullptr);
        return *res_;
    }

    TRes* operator->() const {
        assert(res_ != nullptr);
        return res_;
    }

private:
    TRes* res_;
};

}   // namespace detail

class GFXResView : public detail::GFXResViewBase<GFXRes> {
public:
    GFXResView() = default;
    GFXResView(GFXRes& res)
        : detail::GFXResViewBase<GFXRes>(res) {}
};

class GFXResCView : public detail::GFXResViewBase<const GFXRes> {
public:
    GFXResCView() = default;
    GFXResCView(const GFXRes& res)
        : detail::GFXResViewBase<const GFXRes>(res) {}
};

class GFXStorage;

namespace detail {

template <class T>
class GFXResGen {
private:
    struct Promise;

public:
    using promise_type = Promise;

    GFXResGen() = default;
    explicit GFXResGen(std::coroutine_handle<Promise> h)
        : h_(h) {}

    GFXResGen(const GFXResGen& other) = default;
    GFXResGen& operator=(const GFXResGen& other) = default;

    GFXResGen(GFXResGen&& other) noexcept
        : h_( std::exchange(other.h_, nullptr) ) {}

    GFXResGen& operator=(GFXResGen&& other) noexcept {
        if (this != &other) {
            h_ = std::exchange(other.h_, nullptr);
        }

        return *this;
    }

    ~GFXResGen() {
        destroy();
    }

    void destroy() noexcept {
        if (h_) {
            h_.destroy();
            h_ = nullptr;
        }
    }

    T gen() {
        resume(*this);
        return h_.promise().get();
    }

private:
    static void resume(GFXResGen& gen);

    std::coroutine_handle<Promise> h_;
};

template <class T>
struct GFXResGen<T>::Promise {
    GFXResGen get_return_object() noexcept {
        return GFXResGen( std::coroutine_handle<Promise>
            ::from_promise(*this)
        );
    }

    std::suspend_always initial_suspend() noexcept {
        return {};
    }

    std::suspend_never final_suspend() noexcept {
        return {};
    }

    void unhandled_exception() noexcept {
        stored_ = std::current_exception();
    }

    void return_void() noexcept {}

    std::suspend_always yield_value(T value) noexcept {
        stored_ = std::move(value);
        return {};
    }

    T& peek() {
        assert(std::holds_alternative<T>(stored_));
        return std::get<T>(stored_);
    }

    T get() {
        assert(std::holds_alternative<T>(stored_));
        return std::move( std::get<T>(stored_) );
    }

    std::variant< std::monostate, T, std::exception_ptr > stored_;
};

template <class T>
void GFXResGen<T>::resume(GFXResGen& gen) {
    gen.h_.resume();

    if ( std::holds_alternative<std::exception_ptr>(
        gen.h_.promise().stored_
    ) ) [[unlikely]] {
        std::rethrow_exception(
            std::get<std::exception_ptr>(gen.h_.promise().stored_)
        );
    }
}

}

class GFXRes {
private:
    template <class T>
    struct Type {};

    struct Dummy {};
    static std::optional<Dummy> nullTag;
    
    enum class GenMode {
        StandAlone, Load, Cache
    };

public:
    friend class GFXStorage;

    using ID = int;

    struct Pair {
        ID id;
        IBindable* bindee;  
    };

    GFXRes()
        : gen_(), stored_(), genMode_(GenMode::StandAlone) {}

    ~GFXRes() {
        if (genMode_ == GenMode::StandAlone && valid()) {
            delete stored_.value().bindee;
        }
    }

    GFXRes(const GFXRes& other) = delete;
    GFXRes& operator=(const GFXRes& other) = delete;
    GFXRes(GFXRes&& other) noexcept;
    GFXRes& operator=(GFXRes&& other) noexcept;

    template <class T, class ... Args>
    static GFXRes makeStandAlone(Args&& ... args) {
        return GFXRes( Type<T>{}, GenMode::StandAlone, std::nullopt,
            nullTag, std::forward<Args>(args)...
        );
    }

    template <class T, class ... Args>
    static GFXRes makeLoaded(GFXStorage& storage, Args&& ... args) {
        return GFXRes( Type<T>{}, GenMode::Load, storage,
            nullTag, std::forward<Args>(args)...
        );
    }

    template <class T, class Tag, class ... Args>
    static GFXRes makeCached(GFXStorage& storage, Tag tag, Args&& ... args) {
        return GFXRes( Type<T>{}, GenMode::Cache, storage,
            std::optional<Tag>(tag), std::forward<Args>(args)...
        );
    }

    ID id() const {
        if (!valid()) [[unlikely]] {
            reconstruct();
        }

        return stored_.value().id;
    }

    IBindable& get() {
        if (!valid()) [[unlikely]] {
            reconstruct();
        }

        return *stored_.value().bindee;
    }

    const IBindable& get() const {
        if (!valid()) [[unlikely]] {
            reconstruct();
        }

        return *stored_.value().bindee;
    }

    template <class T>
    T& as() {
        return static_cast<T&>(get());
    }

    template <class T>
    const T& as() const {
        return static_cast<const T&>(get());
    }

    IBindable* operator->() {
        return &get();
    }

    const IBindable* operator->() const {
        return &get();
    }

    const GFXResView view() noexcept {
        return GFXResView(*this);
    }

    const GFXResCView view() const noexcept {
        return cview();
    }

    const GFXResCView cview() const noexcept {
        return GFXResCView(*this);
    }

    bool valid() const noexcept {
        return stored_.has_value();
    }

    void destroy() {
        // to be implemented
    }

private:
    template <class T, class Tag, class ... Args>
    GFXRes( Type<T>, GenMode gmod,
        std::optional<std::reference_wrapper<GFXStorage>> storage,
        std::optional<Tag> tag,
        Args&& ... args
    );

    template <class T, class ... Args>
        requires std::is_base_of_v<IBindable, T>
    detail::GFXResGen<Pair> makeGenStandAlone(Args ... args);

    template <class T, class ... Args>
        requires std::is_base_of_v<IBindable, T>
    detail::GFXResGen<Pair> makeGenStorageLoad(GFXStorage& storage, Args ... args);

    template <class T, class Tag, class ... Args>
        requires std::is_base_of_v<IBindable, T>
    detail::GFXResGen<Pair> makeGenStorageCache(GFXStorage& storage, Tag tag, Args ... args);

    template <class Arg>
    auto packArg(Arg&& arg) {
        if constexpr (std::is_lvalue_reference_v<Arg>) {
            return std::ref(arg);
        }
        else {
            return std::move(arg);
        }
    }

    template <class Arg>
    decltype(auto) unpackArg(std::reference_wrapper<Arg>& arg) {
        return arg.get();
    }

    template <class Arg>
    decltype(auto) unpackArg(Arg& arg) {
        return arg;
    }

    void invalidate() noexcept {
        stored_.reset();
    }

    void reconstruct() const {
        stored_ = gen_.gen();
    }

    mutable detail::GFXResGen<Pair> gen_;
    mutable std::optional<Pair> stored_;
    GenMode genMode_;
};

template <class T, class Tag, class ... Args>
GFXRes::GFXRes( Type<T>, GenMode gmod,
    std::optional<std::reference_wrapper<GFXStorage>> storage,
    std::optional<Tag> tag,
    Args&& ... args
) : gen_(), stored_(), genMode_(gmod) {
    switch (gmod) {
    case GenMode::StandAlone:
        gen_ = makeGenStandAlone<T>( 
            packArg(std::forward<Args>(args))...
        );
        break; 

    case GenMode::Load:
        assert(storage.has_value());
        gen_ = makeGenStorageLoad<T>( 
            storage.value().get(),
            packArg(std::forward<Args>(args))...
        );
        break; 

    case GenMode::Cache:
        assert(storage.has_value());
        assert(tag.has_value());
        gen_ = makeGenStorageCache<T>(
            storage.value().get(), tag.value(),
            packArg(std::forward<Args>(args))...
        );
        break; 

    default:
        // do proper error handling
        break;
    }

    stored_ = gen_.gen();
}

template <class T>
concept GFXResRepresentable = std::is_base_of_v<GFXRes, std::remove_cvref_t<T>>
    || std::is_base_of_v<GFXResView, std::remove_cvref_t<T>>
    || std::is_base_of_v<GFXResCView, std::remove_cvref_t<T>>;

namespace detail {

inline const GFXRes::ID makeGFXResID() noexcept {
    static auto IDDistribution = 0;
    // maybe occur overflow.
    // subsitute this with robust algorithm later.
    return IDDistribution++;
}

}   // namespace detail

class GFXStorage {
public:
    using ID = GFXRes::ID;
    using Pair = GFXRes::Pair;

    class ManagedBindable {
    public:
        ManagedBindable()
            : bindee_(nullptr), owner_(nullptr) {}

        ManagedBindable(IBindable* bindee, GFXRes* owner)
            : bindee_(bindee), owner_(owner) {}

        ~ManagedBindable() {
            delete bindee_;
            if (owner_) {
                owner_->invalidate();
            }
        }

        ManagedBindable(const ManagedBindable&) = delete;
        ManagedBindable& operator=(const ManagedBindable&) = delete;

        ManagedBindable(ManagedBindable&& other) noexcept
            : bindee_( std::exchange(other.bindee_, nullptr) ),
            owner_( std::exchange(other.owner_, nullptr) ) {}

        ManagedBindable& operator=(ManagedBindable&& other) {
            if (this == &other) [[unlikely]] {
                return *this;
            }

            bindee_ = std::exchange(other.bindee_, nullptr);
            owner_ = std::exchange(other.owner_, nullptr);
        }
        

        IBindable* get() const noexcept {
            return bindee_;
        }

    private:
        IBindable* bindee_;
        GFXRes* owner_;
    };

    GFXStorage()
        : IDCache_(IDCACHE_CACHELINE_SIZE, IDCACHE_NUM_CACHELINE),
        resources_() {}

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::load lead to memory leak")]]
    const Pair load(GFXRes* owner, Args&& ... args) {
        auto bindee = new T(std::forward<Args>(args)...);
        auto id = detail::makeGFXResID();
        resources_.try_emplace(id, ManagedBindable(bindee, owner));

        return Pair{id, bindee};
    }

    template <class T, class Tag, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::cache lead to memory leak")]]
    const Pair cache(GFXRes* owner, Tag, Args&& ... args) {
        if ( auto cache_res = IDCache_.at( std::type_index( typeid(Tag) ) ); cache_res.hit() ) {
            const auto& id = cache_res.value();
            const auto& bindee = resources_.at(id).get();
            return Pair{id, bindee};
        }

        const auto ret = load<T>( owner, std::forward<Args>(args)... );
        IDCache_.try_emplace( std::type_index( typeid(Tag) ), ret.id );

        return ret;
    }

    std::optional<IBindable*> get(const ID& id) const noexcept {
        if ( auto it = resources_.find(id); it != resources_.end() ) {
            return it->second.get();
        }
        return std::nullopt;
    }

    template <class Tag>
    std::optional<IBindable*> get() const noexcept {
        return get( typeid(Tag) );
    }

    std::optional<IBindable*> get(std::type_index tagID) const noexcept {
        if ( auto cache_res = IDCache_.at( tagID ); cache_res.hit() ) {
            assert( resources_.contains( cache_res.value() ) );
            return resources_.at( cache_res.value() ).get();
        }
        return std::nullopt;
    }

    bool search(const ID& id) const noexcept {
        return resources_.contains(id);
    }

    template <class Tag>
    bool search() const noexcept {
        return search( typeid(Tag) );
    }

    bool search(std::type_index tagID) const noexcept {
        return IDCache_.contains(tagID);
    }

private:
    static constexpr std::size_t IDCACHE_CACHELINE_SIZE = 0x08u;
    static constexpr std::size_t IDCACHE_NUM_CACHELINE = 0x40u;
    
    LRUCache< std::type_index, ID > IDCache_;
    std::map< ID, ManagedBindable > resources_;
};

template <class T, class ... Args>
    requires std::is_base_of_v<IBindable, T>
detail::GFXResGen<GFXRes::Pair> GFXRes::makeGenStandAlone(Args ... args) {
    for (;;) {
        co_yield Pair{
            .id = detail::makeGFXResID(),
            .bindee = new T(unpackArg(args)...)
        };
    }
}

template <class T, class ... Args>
    requires std::is_base_of_v<IBindable, T>
detail::GFXResGen<GFXRes::Pair> GFXRes::makeGenStorageLoad(
    GFXStorage& storage, Args ... args
) {
    for (;;) {
        co_yield storage.load<T>(this, unpackArg(args)...);
    }
}

template <class T, class Tag, class ... Args>
    requires std::is_base_of_v<IBindable, T>
detail::GFXResGen<GFXRes::Pair> GFXRes::makeGenStorageCache(
    GFXStorage& storage, Tag tag, Args ... args
) {
    for (;;) {
        co_yield storage.cache<T>(this, tag, unpackArg(args)...);
    }
}

#endif  // __GraphicsStorage
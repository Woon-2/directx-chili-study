#ifndef __GraphicsStorage
#define __GraphicsStorage

#include "Bindable.hpp"

#include <map>
#include <memory>

class GFXStorage {
public:
    using ID = int;

    template <class T, class ... Args>
    [[nodiscard("ignoring return value of GFXStorage::load lead to memory leak")]]
    const ID load(Args&& ... args) {
        auto tmp = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
        auto id = makeID();
        resources_.emplace(id, std::move(tmp));

        return id;
    }

    const std::shared_ptr<IBindable> get(const ID& id) const noexcept {
        return resources_.find(id)->second;
    }

private:
    static const ID makeID() noexcept {
        // maybe occur overflow.
        // subsitute this with robust algorithm later.
        return IDDistribution++;
    }

    static int IDDistribution;
    std::map< ID, std::shared_ptr<IBindable> > resources_;
};

int GFXStorage::IDDistribution = 0;

#endif  // __GraphicsStorage
#ifndef __AdditionalRanges
#define __AdditionalRanges

#include "AdditionalConcepts.hpp"
#include <iterator>
#include <type_traits>
#include <ranges>
#include <utility>

template <std::input_or_output_iterator Iter>
    requires pointable< std::iter_value_t<Iter> >
class dereference_iterator {
public:
    using iterator_type = Iter;
    using difference_type = typename iterator_type::difference_type;
    using value_type = std::remove_reference_t< decltype(**std::declval<iterator_type>()) >;
    using pointer = decltype(*std::declval<iterator_type>().operator->());
    using reference = std::add_lvalue_reference_t< value_type >;
    using iterator_category = typename iterator_type::iterator_category;

    dereference_iterator() = default;

    dereference_iterator(iterator_type iter)
        : iter_(iter) {}


    reference operator*() const {
        return **iter_;
    }

    pointer operator->() const {
        return *(iter_.operator->());
    }

    dereference_iterator& operator++() {
        ++iter_;
        return *this;
    }

    dereference_iterator operator++(int) {
        dereference_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    dereference_iterator& operator--()
        requires requires (Iter& iter) {
        --iter;
    } {
        --iter_;
        return *this;
    }

    dereference_iterator operator--(int)
        requires requires (Iter& iter) {
        iter--;
    } {
        dereference_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    dereference_iterator& operator+=(difference_type n)
        requires requires (Iter& iter, difference_type n) {
        iter += n;
    } {
        iter_ += n;
        return *this;
    }

    dereference_iterator& operator-=(difference_type n)
        requires requires (Iter& iter, difference_type n) {
        iter -= n;
    } {
        iter_ -= n;
        return *this;
    }

    dereference_iterator operator+(difference_type n) const
        requires requires (Iter& iter, difference_type n) {
            { iter + n } -> std::same_as<Iter>;
    } {
        return dereference_iterator(iter_ + n);
    }

    dereference_iterator operator-(difference_type n) const
        requires requires (Iter& iter, difference_type n) {
            { iter - n } -> std::same_as<Iter>;
    } {
        return dereference_iterator(iter_ - n);
    }

    reference operator[](difference_type n) const
        requires requires (Iter& iter, difference_type n) {
            { iter[n] } -> std::same_as< std::iter_reference_t<Iter> >;
    } {
        return *(iter_[n]);
    }

    friend dereference_iterator operator+(
        difference_type n, const dereference_iterator& iter
        ) requires requires (Iter& iter, difference_type n) {
            { iter + n } -> std::same_as<Iter>;
    } {
        return iter + n;
    }

    friend difference_type operator-(
        const dereference_iterator& lhs,
        const dereference_iterator& rhs
        ) requires requires (const Iter& lhs, const Iter& rhs) {
            { lhs - rhs } -> std::same_as<difference_type>;
    } {
        return difference_type(lhs.iter_ - rhs.iter_);
    }

    friend bool operator==(const dereference_iterator& lhs,
        const dereference_iterator& rhs
        ) {
        return lhs.iter_ == rhs.iter_;
    }

    friend bool operator!=(const dereference_iterator& lhs,
        const dereference_iterator& rhs
        ) {
        return !(lhs == rhs);
    }

    friend auto operator<=>(const dereference_iterator& lhs,
        const dereference_iterator& rhs
        ) requires requires (const Iter& lhs, const Iter& rhs) {
        lhs <=> rhs;
    } {
        return lhs.iter_ <=> rhs.iter_;
    }

private:
    iterator_type iter_;
};

template <std::ranges::range R>
    requires std::ranges::view<R>
class dereference_view :
    public std::ranges::view_interface<dereference_view<R>> {
public:
    dereference_view() = default;

    constexpr dereference_view(R base)
        : base_(base) {}

    constexpr R base() const& {
        return base_;
    }

    constexpr R base() const&& {
        return std::move(base_);
    }

    constexpr auto begin() const {
        return dereference_iterator(base_.begin());
    }

    constexpr auto end() const {
        return dereference_iterator(base_.end());
    }

private:
    R base_;
};

// is it necessary?
template <class R>
dereference_view(R&& base)->dereference_view<std::views::all_t<R>>;

namespace detail {

    struct dereference_view_adaptor_closure {
        template <std::ranges::viewable_range R>
        constexpr auto operator()(R&& r) const {
            return dereference_view(std::forward<R>(r));
        }
    };

    struct dereference_view_adaptor {
        template <std::ranges::viewable_range R>
        constexpr auto operator()(R&& r) const {
            return dereference_view(std::forward<R>(r));
        }

        constexpr auto operator()() const {
            return dereference_view_adaptor_closure();
        }
    };

    template <std::ranges::viewable_range R>
    constexpr auto operator|(R&& r,
        const dereference_view_adaptor_closure& adaptor
        ) {
        return adaptor(std::forward<R>(r));
    }

}

static detail::dereference_view_adaptor dereference;

template <class R>
concept reservable_range = std::ranges::range<R>
    && requires (R& _r, std::size_t _n) {
        _r.reserve(_n);
    };

template <std::ranges::range R>
[[maybe_unused]] R&& reserve_if_possible(R&& range, std::size_t newCapicity) {
    if constexpr ( reservable_range<R> ) {
        range.reserve(newCapicity);
    }
    return std::forward<R>(range);
}

#endif  // __AdditionalRanges
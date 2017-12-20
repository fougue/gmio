#pragma once

#if 1
#include <cassert>
#include <iterator>
#include <type_traits>

namespace gmio {
namespace internal {

template<typename SPAN, bool IS_CONST> class SpanIterator {
public:
    using element_type = typename SPAN::element_type;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename SPAN::value_type;
    using difference_type = typename SPAN::index_type;

    using reference = std::conditional_t<IS_CONST, const element_type, element_type>&;
    using pointer = std::add_pointer_t<reference>;

    constexpr SpanIterator()
        : m_span(nullptr), m_index(0) {}

    constexpr SpanIterator(const SPAN* span, size_t index)
        : m_span(span), m_index(index) {}

    constexpr reference operator*() const { return *(m_span->data() + m_index); }
    constexpr pointer operator->() const { return m_span->data() + m_index; }

    SpanIterator& operator++() {
        ++m_index;
        return *this;
    }

    SpanIterator operator++(int) {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    SpanIterator& operator--() {
        --m_index;
        return *this;
    }

    SpanIterator operator--(int) {
        auto ret = *this;
        --(*this);
        return ret;
    }

    SpanIterator operator+(difference_type n) const {
        auto ret = *this;
        return ret += n;
    }

    SpanIterator& operator+=(difference_type n) {
        m_index += n;
        return *this;
    }

    constexpr SpanIterator operator-(difference_type n) const {
        auto ret = *this;
        return ret -= n;
    }

    SpanIterator& operator-=(difference_type n)
    { return *this += -n; }

    constexpr difference_type operator-(const SpanIterator& rhs) const
    { return m_index - rhs.m_index; }

    constexpr reference operator[](difference_type n) const
    { return *(*this + n); }

    constexpr friend bool operator==(const SpanIterator& lhs, const SpanIterator& rhs)
    { return (lhs.m_span == rhs.m_span) && (lhs.m_index == rhs.m_index); }

    constexpr friend bool operator!=(const SpanIterator& lhs, const SpanIterator& rhs)
    { return !(lhs == rhs); }

    constexpr friend bool operator<(const SpanIterator& lhs, const SpanIterator& rhs)
    { return lhs.m_index < rhs.m_index; }

    constexpr friend bool operator<=(const SpanIterator& lhs, const SpanIterator& rhs)
    { return !(rhs < lhs); }

    constexpr friend bool operator>(const SpanIterator& lhs, const SpanIterator& rhs)
    { return rhs < lhs; }

    constexpr friend bool operator>=(const SpanIterator& lhs, const SpanIterator& rhs)
    { return !(rhs > lhs); }

private:
    const SPAN* m_span;
    std::ptrdiff_t m_index;
};

template <typename SPAN, bool IS_CONST>
constexpr SpanIterator<SPAN, IS_CONST> operator+(
        typename SpanIterator<SPAN, IS_CONST>::difference_type n,
        const SpanIterator<SPAN, IS_CONST>& rhs)
{
    return rhs + n;
}

template <typename SPAN, bool IS_CONST>
constexpr SpanIterator<SPAN, IS_CONST> operator-(
        typename SpanIterator<SPAN, IS_CONST>::difference_type n,
        const SpanIterator<SPAN, IS_CONST>& rhs)
{
    return rhs - n;
}

} // namespace internal

template<typename T> class Span {
public:
    using element_type = T;
    using value_type = std::remove_const_t<T>;
    using index_type = std::ptrdiff_t;
    using pointer = element_type*;
    using reference = element_type&;

    using iterator = internal::SpanIterator<Span<T>, false>;
    using const_iterator = internal::SpanIterator<Span<T>, true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = index_type;

    constexpr Span()
        : m_ptr(nullptr), m_size(0) {}

    constexpr Span(std::nullptr_t)
        : Span() {}

    constexpr Span(pointer ptr, size_type count)
        : m_ptr(ptr), m_size(count) {}

    constexpr Span(pointer first, pointer last)
        : Span(first, std::distance(first, last)) {}

    template<size_t N> constexpr Span(T (&array)[N])
        : Span(&array[0], N) {}

    template<typename CONTAINER> constexpr Span(const CONTAINER& cnter)
        : Span(cnter.data(), cnter.size()) {}

    template<typename CONTAINER> constexpr Span(CONTAINER& cnter)
        : Span(cnter.data(), cnter.size()) {}

    constexpr Span(const Span& other) = default;
    constexpr Span(Span&& other) = default;

    ~Span() = default;

    Span& operator=(const Span& other) {
        m_ptr = other.m_ptr;
        m_size = other.m_size;
        return *this;
    }
    Span& operator=(Span&& other) = default;

    constexpr size_t size() const { return m_size; }
    constexpr bool empty() const { return m_size == 0; }

    constexpr reference at(index_type i) const { return m_ptr[i]; }
    constexpr reference operator[](index_type i) const { return this->at(i); }
    constexpr reference operator()(index_type i) const { return this->at(i); }
    constexpr pointer data() const { return m_ptr; }

    iterator begin() const { return {this, 0}; }
    iterator end() const { return {this, m_size}; }

    const_iterator cbegin() const { return {this, 0}; }
    const_iterator cend() const { return {this, m_size}; }

    reverse_iterator rbegin() const { return reverse_iterator{end()}; }
    reverse_iterator rend() const { return reverse_iterator{begin()}; }

    const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
    const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }

private:
    T* m_ptr;
    size_t m_size;
};

template<typename T> Span<T> makeSpan(T* ptr, size_t count) {
    return Span<T>(ptr, count);
}

template<typename T> Span<T> makeSpan(T* first, T* last) {
    return Span<T>(first, last);
}

template<typename T, size_t N> Span<T> makeSpan(T (&array)[N]) {
    return Span<T>(array, N);
}

template<typename T, size_t N> Span<const T> makeSpan(const T (&array)[N]) {
    return Span<const T>(array, N);
}

template<typename CONTAINER>
Span<typename CONTAINER::value_type> makeSpan(CONTAINER& cnter) {
    return Span<typename CONTAINER::value_type>(cnter);
}

template<typename CONTAINER>
Span<const typename CONTAINER::value_type> makeSpan(const CONTAINER& cnter)
{
    return Span<const typename CONTAINER::value_type>(cnter);
}

} // namespace gmio

#else
#include "gsl_span.h"

namespace gmio {

template <typename T, std::ptrdiff_t EXTENT = gsl::dynamic_extent> using Span = gsl::span<T, EXTENT>;

//
// makeSpan() - Utility functions for creating spans
//
template <typename T>
Span<T> makeSpan(T* ptr, typename Span<T>::index_type count)
{
    return gsl::make_span(ptr, count);
}

template <typename T>
Span<T> makeSpan(T* firstElem, T* lastElem)
{
    return gsl::make_span(firstElem, lastElem);
}

template <typename T, std::size_t N>
Span<T, N> makeSpan(T (&arr)[N])
{
    return gsl::make_span(arr);
}

template <typename CONTAINER>
Span<typename CONTAINER::value_type> makeSpan(CONTAINER& cont)
{
    return gsl::make_span(cont);
}

template <typename CONTAINER>
Span<const typename CONTAINER::value_type> makeSpan(const CONTAINER& cont)
{
    return gsl::make_span(cont);
}

template <typename PTR>
Span<typename PTR::element_type> makeSpan(PTR& cont, std::ptrdiff_t count)
{
    return gsl::make_span(cont, count);
}

template <typename PTR>
Span<typename PTR::element_type> makeSpan(PTR& cont)
{
    return gsl::make_span(cont);
}

} // namespace gmio
#endif

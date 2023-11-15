//
// Created by michele on 31.10.23.
//

#ifndef _UTILS_H_
#define _UTILS_H_

#include <optional>
#include <ostream>

#define __maybe_unused __attribute__((unused))

/**
 * compare_non_empty - Returns the `max` non-empty optional based on the given comparator
 */
template<typename T, typename Comparator>
std::optional<T> compare_non_empty(const std::optional<T> &a, const std::optional<T> &b, Comparator comparator)
{
    if (a.has_value() && b.has_value())
        return comparator(a.value(), b.value()) ? a : b;
    else if (a.has_value())
        return a;
    else if (b.has_value())
        return b;
    else
        return std::nullopt;
}

#endif//_UTILS_H_

#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_HASH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_HASH_HPP_INCLUDED

#include <cstdint>
#include <functional>
#include <type_traits>

namespace approximate_streaming_detail {

inline std::uint64_t splitmix64(std::uint64_t value) noexcept {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}

template<class Key>
std::uint64_t key_hash(const Key& key, std::uint64_t seed) {
    const auto hashed = static_cast<std::uint64_t>(std::hash<Key>{}(key));
    return splitmix64(hashed + seed);
}

inline long double unit_open(std::uint64_t value) noexcept {
    constexpr long double scale = 1.0L / 9007199254740992.0L;
    return (static_cast<long double>((value >> 11U) + 1ULL) - 0.5L) * scale;
}

}  // namespace approximate_streaming_detail

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_HASH_HPP_INCLUDED

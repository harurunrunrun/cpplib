#ifndef CPPLIB_SRC_APPROXIMATE_GEOMETRY_SEED_AND_EXTEND_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GEOMETRY_SEED_AND_EXTEND_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace approximate::geometry {

struct SeedExtendMatch {
    std::size_t text_position = 0;
    std::size_t mismatches = 0;

    friend bool operator==(
        const SeedExtendMatch&,
        const SeedExtendMatch&
    ) = default;
};

namespace detail {

inline constexpr std::uint64_t seed_hash_base =
    UINT64_C(11400714819323198485);
inline constexpr std::uint64_t seed_hash_salt =
    UINT64_C(0x9e3779b97f4a7c15);

template<class Symbol>
std::uint64_t seed_symbol_hash(const Symbol& symbol) {
    return static_cast<std::uint64_t>(
        std::hash<Symbol>{}(symbol)
    ) + seed_hash_salt;
}

template<class Symbol>
std::uint64_t seed_initial_hash(
    std::span<const Symbol> sequence,
    std::size_t length
) {
    std::uint64_t result = 0;
    for(std::size_t index = 0; index < length; ++index){
        result = result * seed_hash_base
            + seed_symbol_hash(sequence[index]);
    }
    return result;
}

template<class Symbol>
bool seed_equal(
    std::span<const Symbol> text,
    std::size_t text_position,
    std::span<const Symbol> pattern,
    std::size_t pattern_position,
    std::size_t seed_length
) {
    return std::equal(
        pattern.begin()
            + static_cast<std::ptrdiff_t>(pattern_position),
        pattern.begin()
            + static_cast<std::ptrdiff_t>(
                pattern_position + seed_length
            ),
        text.begin()
            + static_cast<std::ptrdiff_t>(text_position)
    );
}

template<class Symbol>
std::size_t hamming_mismatches(
    std::span<const Symbol> text,
    std::size_t text_position,
    std::span<const Symbol> pattern,
    std::size_t maximum_mismatches
) {
    std::size_t mismatches = 0;
    for(std::size_t index = 0;
        index < pattern.size(); ++index){
        if(text[text_position + index] == pattern[index]){
            continue;
        }
        ++mismatches;
        if(mismatches > maximum_mismatches) break;
    }
    return mismatches;
}

}  // namespace detail

template<class Symbol>
requires requires(const Symbol& symbol) {
    { std::hash<Symbol>{}(symbol) }
        -> std::convertible_to<std::size_t>;
    { symbol == symbol } -> std::convertible_to<bool>;
}
[[nodiscard]] std::vector<SeedExtendMatch> seed_and_extend(
    std::span<const Symbol> text,
    std::span<const Symbol> pattern,
    std::size_t seed_length,
    std::size_t maximum_mismatches
) {
    if(pattern.empty()){
        throw std::invalid_argument(
            "seed-and-extend pattern must not be empty"
        );
    }
    if(seed_length == 0 || seed_length > pattern.size()){
        throw std::invalid_argument(
            "seed length must be in [1, pattern length]"
        );
    }
    if(pattern.size() > text.size()) return {};

    const std::size_t alignment_count =
        text.size() - pattern.size() + 1;
    std::vector<unsigned char> candidate(alignment_count, 0);
    std::unordered_map<
        std::uint64_t, std::vector<std::size_t>
    > text_seeds;
    text_seeds.reserve(text.size() - seed_length + 1);
    std::uint64_t power = 1;
    for(std::size_t index = 1;
        index < seed_length; ++index){
        power *= detail::seed_hash_base;
    }
    std::uint64_t hash =
        detail::seed_initial_hash(text, seed_length);
    for(std::size_t position = 0;
        position + seed_length <= text.size();
        ++position){
        text_seeds[hash].push_back(position);
        if(position + seed_length == text.size()) break;
        hash -= detail::seed_symbol_hash(text[position])
            * power;
        hash = hash * detail::seed_hash_base
            + detail::seed_symbol_hash(
                text[position + seed_length]
            );
    }

    std::uint64_t pattern_hash =
        detail::seed_initial_hash(pattern, seed_length);
    for(std::size_t pattern_position = 0;
        pattern_position + seed_length <= pattern.size();
        ++pattern_position){
        const auto found = text_seeds.find(pattern_hash);
        if(found != text_seeds.end()){
            for(const std::size_t text_position :
                found->second){
                if(text_position < pattern_position){
                    continue;
                }
                const std::size_t alignment =
                    text_position - pattern_position;
                if(alignment >= alignment_count
                   || candidate[alignment] != 0){
                    continue;
                }
                if(detail::seed_equal(
                       text, text_position,
                       pattern, pattern_position,
                       seed_length
                   )){
                    candidate[alignment] = 1;
                }
            }
        }
        if(pattern_position + seed_length
           == pattern.size()){
            break;
        }
        pattern_hash -= detail::seed_symbol_hash(
            pattern[pattern_position]
        ) * power;
        pattern_hash = pattern_hash
            * detail::seed_hash_base
            + detail::seed_symbol_hash(
                pattern[pattern_position + seed_length]
            );
    }

    const bool exact_seed_is_guaranteed =
        maximum_mismatches < pattern.size()
        && seed_length <=
            (pattern.size() - maximum_mismatches)
                / (maximum_mismatches + 1);
    if(!exact_seed_is_guaranteed){
        std::fill(candidate.begin(), candidate.end(), 1);
    }

    std::vector<SeedExtendMatch> result;
    for(std::size_t position = 0;
        position < alignment_count; ++position){
        if(candidate[position] == 0) continue;
        const std::size_t mismatches =
            detail::hamming_mismatches(
                text, position, pattern,
                maximum_mismatches
            );
        if(mismatches <= maximum_mismatches){
            result.push_back({position, mismatches});
        }
    }
    return result;
}

template<class Symbol>
[[nodiscard]] std::vector<SeedExtendMatch> seed_and_extend(
    const std::vector<Symbol>& text,
    const std::vector<Symbol>& pattern,
    std::size_t seed_length,
    std::size_t maximum_mismatches
) {
    return seed_and_extend<Symbol>(
        std::span<const Symbol>(text),
        std::span<const Symbol>(pattern),
        seed_length,
        maximum_mismatches
    );
}

}  // namespace approximate::geometry

#endif  // CPPLIB_SRC_APPROXIMATE_GEOMETRY_SEED_AND_EXTEND_HPP_INCLUDED

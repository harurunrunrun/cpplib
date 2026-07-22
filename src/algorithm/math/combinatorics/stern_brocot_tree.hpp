#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STERN_BROCOT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STERN_BROCOT_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>

namespace math{

template<std::signed_integral Int = long long, int MAX_RUNS = 128>
struct SternBrocotTree{
    static_assert(sizeof(Int) <= 8);
    static_assert(MAX_RUNS >= 0);

    using integer_type = Int;
    using wide_type = __int128_t;

    struct Fraction{
        Int numerator = 1;
        Int denominator = 1;

        friend bool operator==(const Fraction&, const Fraction&) = default;
    };

    struct WideFraction{
        wide_type numerator;
        wide_type denominator;
    };

    enum class Direction : unsigned char{
        Left,
        Right,
    };

    struct Run{
        Direction direction;
        std::uint64_t length;

        friend bool operator==(const Run&, const Run&) = default;
    };

    class Path{
    private:
        std::array<Run, static_cast<std::size_t>(MAX_RUNS)> _runs{};
        int _size = 0;

    public:
        static constexpr int capacity() noexcept{ return MAX_RUNS; }

        int size() const noexcept{ return _size; }

        bool empty() const noexcept{ return _size == 0; }

        const Run* begin() const noexcept{ return _runs.data(); }

        const Run* end() const noexcept{ return _runs.data() + _size; }

        const Run& operator[](int index) const{
            if(index < 0 || _size <= index)[[unlikely]]{
                throw std::out_of_range("library assertion fault: range violation (SternBrocotTree::Path). ");
            }
            return _runs[static_cast<std::size_t>(index)];
        }

        void clear() noexcept{ _size = 0; }

        void push_back(Direction direction, std::uint64_t length){
            if(length == 0)[[unlikely]]{
                throw std::invalid_argument("library assertion fault: zero run (SternBrocotTree::Path). ");
            }
            if(_size != 0 && _runs[static_cast<std::size_t>(_size - 1)].direction == direction){
                auto& last = _runs[static_cast<std::size_t>(_size - 1)].length;
                if(last > std::numeric_limits<std::uint64_t>::max() - length)[[unlikely]]{
                    throw std::overflow_error("library assertion fault: depth overflow (SternBrocotTree::Path). ");
                }
                last += length;
                return;
            }
            if(_size == MAX_RUNS)[[unlikely]]{
                throw std::length_error("library assertion fault: capacity violation (SternBrocotTree::Path). ");
            }
            _runs[static_cast<std::size_t>(_size++)] = Run{direction, length};
        }

        std::uint64_t depth() const{
            std::uint64_t result = 0;
            for(const Run& run: *this){
                if(result > std::numeric_limits<std::uint64_t>::max() - run.length)[[unlikely]]{
                    throw std::overflow_error("library assertion fault: depth overflow (SternBrocotTree::Path). ");
                }
                result += run.length;
            }
            return result;
        }
    };

    struct Range{
        Fraction lower;
        Fraction upper;

        friend bool operator==(const Range&, const Range&) = default;
    };

private:
    struct State{
        WideFraction lower{0, 1};
        WideFraction current{1, 1};
        WideFraction upper{1, 0};
    };

    static constexpr wide_type int_maximum(){
        return static_cast<wide_type>(std::numeric_limits<Int>::max());
    }

    static Int narrow(wide_type value){
        if(value < 0 || int_maximum() < value)[[unlikely]]{
            throw std::overflow_error("library assertion fault: integer overflow (SternBrocotTree). ");
        }
        return static_cast<Int>(value);
    }

    static Fraction narrow_fraction(const WideFraction& value){
        return Fraction{narrow(value.numerator), narrow(value.denominator)};
    }

    static void validate_node(const Fraction& fraction){
        if(!is_node(fraction))[[unlikely]]{
            throw std::invalid_argument("library assertion fault: invalid node (SternBrocotTree). ");
        }
    }

    static WideFraction add_multiple(
        const WideFraction& base,
        const WideFraction& step,
        std::uint64_t count
    ){
        const wide_type multiplier = static_cast<wide_type>(count);
        return WideFraction{
            base.numerator + multiplier * step.numerator,
            base.denominator + multiplier * step.denominator,
        };
    }

    static void check_state(const State& state){
        (void)narrow_fraction(state.lower);
        (void)narrow_fraction(state.current);
        (void)narrow_fraction(state.upper);
    }

    static State decode_state(const Path& path){
        State state;
        for(const Run& run: path){
            if(run.direction == Direction::Left){
                state.upper = add_multiple(state.current, state.lower, run.length - 1);
                state.current = add_multiple(state.current, state.lower, run.length);
            }else{
                state.lower = add_multiple(state.current, state.upper, run.length - 1);
                state.current = add_multiple(state.current, state.upper, run.length);
            }
            check_state(state);
        }
        return state;
    }

    static Path common_prefix(const Path& lhs, const Path& rhs){
        Path result;
        const int count = std::min(lhs.size(), rhs.size());
        for(int i = 0; i < count; ++i){
            const Run& left = lhs[i];
            const Run& right = rhs[i];
            if(left.direction != right.direction) break;
            result.push_back(left.direction, std::min(left.length, right.length));
            if(left.length != right.length) break;
        }
        return result;
    }

    static Path suffix_after_depth(const Path& path, std::uint64_t skipped){
        Path result;
        for(const Run& run: path){
            if(skipped >= run.length){
                skipped -= run.length;
                continue;
            }
            result.push_back(run.direction, run.length - skipped);
            skipped = 0;
        }
        if(skipped != 0)[[unlikely]]{
            throw std::logic_error("library assertion fault: invalid path prefix (SternBrocotTree). ");
        }
        return result;
    }

    static Fraction simplest_after_ancestor(
        const Path& ancestor_path,
        const Path& descendant_path,
        Direction away_from_ancestor,
        Direction toward_ancestor
    ){
        const Path suffix = suffix_after_depth(descendant_path, ancestor_path.depth());
        if(suffix.empty() || suffix[0].direction != away_from_ancestor)[[unlikely]]{
            throw std::logic_error("library assertion fault: invalid ancestor order (SternBrocotTree). ");
        }

        if(suffix[0].length > 1){
            Path result = ancestor_path;
            result.push_back(away_from_ancestor, 1);
            return from_path(result);
        }
        if(suffix.size() >= 3){
            Path result = ancestor_path;
            result.push_back(away_from_ancestor, 1);
            result.push_back(suffix[1].direction, suffix[1].length);
            return from_path(result);
        }

        Path result = descendant_path;
        result.push_back(toward_ancestor, 1);
        return from_path(result);
    }

public:
    static bool is_node(const Fraction& fraction) noexcept{
        if(fraction.numerator <= 0 || fraction.denominator <= 0) return false;
        return std::gcd(fraction.numerator, fraction.denominator) == Int(1);
    }

    static Fraction reduce(Fraction fraction){
        if(fraction.numerator <= 0 || fraction.denominator <= 0)[[unlikely]]{
            throw std::invalid_argument("library assertion fault: invalid fraction (SternBrocotTree::reduce). ");
        }
        const Int divisor = std::gcd(fraction.numerator, fraction.denominator);
        fraction.numerator /= divisor;
        fraction.denominator /= divisor;
        return fraction;
    }

    static int compare(const Fraction& lhs, const Fraction& rhs){
        const auto valid = [](const Fraction& value){
            return value.numerator >= 0 && value.denominator >= 0 &&
                (value.numerator != 0 || value.denominator != 0);
        };
        if(!valid(lhs) || !valid(rhs))[[unlikely]]{
            throw std::invalid_argument("library assertion fault: invalid extended fraction (SternBrocotTree::compare). ");
        }
        const wide_type left = static_cast<wide_type>(lhs.numerator) * rhs.denominator;
        const wide_type right = static_cast<wide_type>(rhs.numerator) * lhs.denominator;
        return (right < left) - (left < right);
    }

    static Path encode_path(Fraction fraction){
        validate_node(fraction);
        wide_type numerator = fraction.numerator;
        wide_type denominator = fraction.denominator;
        Path result;
        while(numerator != denominator){
            if(numerator < denominator){
                const wide_type count = (denominator - 1) / numerator;
                result.push_back(Direction::Left, static_cast<std::uint64_t>(count));
                denominator -= count * numerator;
            }else{
                const wide_type count = (numerator - 1) / denominator;
                result.push_back(Direction::Right, static_cast<std::uint64_t>(count));
                numerator -= count * denominator;
            }
        }
        return result;
    }

    static Fraction from_path(const Path& path){
        return narrow_fraction(decode_state(path).current);
    }

    static Fraction decode_path(const Path& path){
        return from_path(path);
    }

    static std::uint64_t depth(Fraction fraction){
        return encode_path(fraction).depth();
    }

    static std::optional<Fraction> ancestor(Fraction fraction, std::uint64_t target_depth){
        const Path path = encode_path(fraction);
        if(path.depth() < target_depth) return std::nullopt;
        Path prefix;
        std::uint64_t remaining = target_depth;
        for(const Run& run: path){
            if(remaining == 0) break;
            const std::uint64_t take = std::min(remaining, run.length);
            prefix.push_back(run.direction, take);
            remaining -= take;
        }
        return from_path(prefix);
    }

    static Fraction lca(Fraction lhs, Fraction rhs){
        return from_path(common_prefix(encode_path(lhs), encode_path(rhs)));
    }

    static bool is_ancestor(Fraction candidate, Fraction fraction){
        return lca(candidate, fraction) == candidate;
    }

    static Range range(Fraction fraction){
        const State state = decode_state(encode_path(fraction));
        return Range{narrow_fraction(state.lower), narrow_fraction(state.upper)};
    }

    static Fraction descend(Fraction fraction, Direction direction, std::uint64_t steps = 1){
        Path path = encode_path(fraction);
        if(steps != 0) path.push_back(direction, steps);
        return from_path(path);
    }

    static Fraction simplest_in_closed_interval(Fraction lower, Fraction upper){
        lower = reduce(lower);
        upper = reduce(upper);
        if(compare(lower, upper) > 0) std::swap(lower, upper);
        return lca(lower, upper);
    }

    static Fraction simplest_in_open_interval(Fraction lower, Fraction upper){
        lower = reduce(lower);
        upper = reduce(upper);
        if(compare(lower, upper) >= 0)[[unlikely]]{
            throw std::invalid_argument("library assertion fault: empty interval (SternBrocotTree::simplest_in_open_interval). ");
        }

        const Path lower_path = encode_path(lower);
        const Path upper_path = encode_path(upper);
        const Path prefix = common_prefix(lower_path, upper_path);
        const std::uint64_t common_depth = prefix.depth();
        const std::uint64_t lower_depth = lower_path.depth();
        const std::uint64_t upper_depth = upper_path.depth();
        if(common_depth < lower_depth && common_depth < upper_depth){
            return from_path(prefix);
        }
        if(common_depth == lower_depth){
            return simplest_after_ancestor(
                lower_path,
                upper_path,
                Direction::Right,
                Direction::Left
            );
        }
        return simplest_after_ancestor(
            upper_path,
            lower_path,
            Direction::Left,
            Direction::Right
        );
    }

    template<class Predicate>
    static Fraction search(Int max_component, Predicate&& is_less_than_target){
        if(max_component <= 0)[[unlikely]]{
            throw std::invalid_argument("library assertion fault: invalid bound (SternBrocotTree::search). ");
        }
        WideFraction lower{0, 1};
        WideFraction upper{1, 0};
        auto&& predicate = is_less_than_target;

        while(true){
            const WideFraction current = add_multiple(lower, upper, 1);
            const bool current_side = static_cast<bool>(std::invoke(predicate, current));
            WideFraction* from = current_side ? &lower : &upper;
            WideFraction* to = current_side ? &upper : &lower;
            std::uint64_t low = 1;
            std::uint64_t high = 2;

            while(static_cast<bool>(std::invoke(predicate, add_multiple(*from, *to, high))) == current_side){
                low = high;
                const WideFraction reached = add_multiple(*from, *to, low);
                if(reached.numerator > max_component || reached.denominator > max_component){
                    return narrow_fraction(*to);
                }
                if(high > std::numeric_limits<std::uint64_t>::max() / 2)[[unlikely]]{
                    throw std::overflow_error("library assertion fault: search overflow (SternBrocotTree). ");
                }
                high *= 2;
            }

            while(low + 1 < high){
                const std::uint64_t middle = low + (high - low) / 2;
                if(static_cast<bool>(std::invoke(predicate, add_multiple(*from, *to, middle))) == current_side){
                    low = middle;
                }else{
                    high = middle;
                }
            }
            *from = add_multiple(*from, *to, low);
        }
    }
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STERN_BROCOT_TREE_HPP_INCLUDED

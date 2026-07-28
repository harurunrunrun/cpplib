#ifndef CPPLIB_SRC_STRUCTURE_TYPES_RATIONAL_RATIONAL_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TYPES_RATIONAL_RATIONAL_HPP_INCLUDED

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <functional>
#include <iomanip>
#include <ios>
#include <istream>
#include <limits>
#include <cstring>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef CP_RATIONAL_KARATSUBA_THRESHOLD
#define CP_RATIONAL_KARATSUBA_THRESHOLD 32
#endif

#ifndef CP_RATIONAL_KARATSUBA_BASECASE
#define CP_RATIONAL_KARATSUBA_BASECASE 40
#endif

#ifndef CP_RATIONAL_REUSE_KARATSUBA_BUFFERS
#define CP_RATIONAL_REUSE_KARATSUBA_BUFFERS 1
#endif

namespace cp {
namespace detail {

template <class T>
struct is_extended_integral_impl : std::is_integral<T> {};

template <class T>
struct is_extended_signed_impl : std::is_signed<T> {};

template <class T>
struct make_unsigned_extended_impl {
    using type = std::make_unsigned_t<T>;
};

#if defined(__SIZEOF_INT128__)
template <>
struct is_extended_integral_impl<__int128> : std::true_type {};
template <>
struct is_extended_integral_impl<unsigned __int128> : std::true_type {};
template <>
struct is_extended_signed_impl<__int128> : std::true_type {};
template <>
struct is_extended_signed_impl<unsigned __int128> : std::false_type {};
template <>
struct make_unsigned_extended_impl<__int128> {
    using type = unsigned __int128;
};
template <>
struct make_unsigned_extended_impl<unsigned __int128> {
    using type = unsigned __int128;
};
#endif

template <class T>
inline constexpr bool is_extended_integral_v =
    is_extended_integral_impl<std::remove_cv_t<T>>::value;

template <class T>
inline constexpr bool is_extended_signed_v =
    is_extended_signed_impl<std::remove_cv_t<T>>::value;

template <class T>
using make_unsigned_extended_t =
    typename make_unsigned_extended_impl<std::remove_cv_t<T>>::type;

template <class T>
using enable_integral_t = std::enable_if_t<is_extended_integral_v<T>, int>;

template <class T>
using enable_floating_t =
    std::enable_if_t<std::is_floating_point_v<std::remove_cv_t<T>>, int>;

template <class T, std::size_t InlineCapacity>
class SmallVector {
    static_assert(std::is_trivially_copyable_v<T>, "SmallVector requires a trivial type");
    static_assert(InlineCapacity != 0, "InlineCapacity must be positive");

public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    SmallVector() noexcept : data_(inline_data()) {}

    SmallVector(const SmallVector& other) : data_(inline_data()) {
        assign(other.begin(), other.end());
    }

    SmallVector(SmallVector&& other) noexcept : data_(inline_data()) {
        move_from(std::move(other));
    }

    SmallVector& operator=(const SmallVector& other) {
        if (this != &other) assign(other.begin(), other.end());
        return *this;
    }

    SmallVector& operator=(SmallVector&& other) noexcept {
        if (this != &other) {
            release_heap();
            data_ = inline_data();
            size_ = 0;
            capacity_ = InlineCapacity;
            move_from(std::move(other));
        }
        return *this;
    }

    ~SmallVector() { release_heap(); }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
    [[nodiscard]] T* data() noexcept { return data_; }
    [[nodiscard]] const T* data() const noexcept { return data_; }
    [[nodiscard]] iterator begin() noexcept { return data_; }
    [[nodiscard]] const_iterator begin() const noexcept { return data_; }
    [[nodiscard]] const_iterator cbegin() const noexcept { return data_; }
    [[nodiscard]] iterator end() noexcept { return data_ + size_; }
    [[nodiscard]] const_iterator end() const noexcept { return data_ + size_; }
    [[nodiscard]] const_iterator cend() const noexcept { return data_ + size_; }

    T& operator[](std::size_t index) noexcept { return data_[index]; }
    const T& operator[](std::size_t index) const noexcept { return data_[index]; }
    T& back() noexcept { return data_[size_ - 1]; }
    const T& back() const noexcept { return data_[size_ - 1]; }

    void clear() noexcept { size_ = 0; }

    void reserve(std::size_t requested) {
        if (requested <= capacity_) return;
        std::size_t next = capacity_ + capacity_ / 2U + 1U;
        if (next < requested) next = requested;
        T* replacement = new T[next];
        if (size_ != 0) std::memcpy(replacement, data_, size_ * sizeof(T));
        release_heap();
        data_ = replacement;
        capacity_ = next;
    }

    void resize(std::size_t requested) { resize(requested, T{}); }

    void resize(std::size_t requested, const T& value) {
        if (requested > capacity_) reserve(requested);
        if (requested > size_) std::fill(data_ + size_, data_ + requested, value);
        size_ = requested;
    }

    void assign(std::size_t count, const T& value) {
        if (count > capacity_) reserve(count);
        std::fill(data_, data_ + count, value);
        size_ = count;
    }

    template <class Iterator>
    void assign(Iterator first, Iterator last) {
        const auto distance = static_cast<std::size_t>(last - first);
        if (distance > capacity_) reserve(distance);
        if (distance != 0) std::copy(first, last, data_);
        size_ = distance;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) reserve(size_ + 1U);
        data_[size_++] = value;
    }

    void pop_back() noexcept { --size_; }

    friend bool operator==(const SmallVector& lhs, const SmallVector& rhs) noexcept {
        return lhs.size_ == rhs.size_
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    friend bool operator!=(const SmallVector& lhs, const SmallVector& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    T inline_storage_[InlineCapacity];
    T* data_;
    std::size_t size_ = 0;
    std::size_t capacity_ = InlineCapacity;

    [[nodiscard]] T* inline_data() noexcept { return inline_storage_; }
    [[nodiscard]] const T* inline_data() const noexcept { return inline_storage_; }
    [[nodiscard]] bool using_inline() const noexcept { return data_ == inline_data(); }

    void release_heap() noexcept {
        if (!using_inline()) delete[] data_;
    }

    void move_from(SmallVector&& other) noexcept {
        if (other.using_inline()) {
            if (other.size_ != 0) std::memcpy(data_, other.data_, other.size_ * sizeof(T));
            size_ = other.size_;
            other.size_ = 0;
        } else {
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = other.inline_data();
            other.size_ = 0;
            other.capacity_ = InlineCapacity;
        }
    }
};

class Integer {
public:
    using limb_type = std::uint32_t;
    static constexpr limb_type base = 1'000'000'000U;
    static constexpr int base_digits = 9;

    Integer() = default;

    template <class T, enable_integral_t<T> = 0>
    Integer(T value) {
        assign_integral(value);
    }

    explicit Integer(std::string_view text) {
        read(text);
    }

    Integer& operator=(std::string_view text) {
        read(text);
        return *this;
    }

    template <class T, enable_integral_t<T> = 0>
    Integer& operator=(T value) {
        assign_integral(value);
        return *this;
    }

    [[nodiscard]] int sign() const noexcept { return sign_; }
    [[nodiscard]] bool is_zero() const noexcept { return sign_ == 0; }
    [[nodiscard]] bool is_negative() const noexcept { return sign_ < 0; }
    [[nodiscard]] bool is_positive() const noexcept { return sign_ > 0; }
    [[nodiscard]] bool is_one() const noexcept { return sign_ == 1 && limbs_.size() == 1 && limbs_[0] == 1U; }
    [[nodiscard]] bool is_minus_one() const noexcept { return sign_ == -1 && limbs_.size() == 1 && limbs_[0] == 1U; }
    explicit operator bool() const noexcept { return !is_zero(); }

    [[nodiscard]] std::size_t limb_count() const noexcept { return limbs_.size(); }
    [[nodiscard]] bool has_single_limb() const noexcept { return limbs_.size() == 1; }
    [[nodiscard]] bool same_magnitude(const Integer& other) const noexcept { return limbs_ == other.limbs_; }
    [[nodiscard]] limb_type single_limb() const noexcept { return limbs_[0]; }

    [[nodiscard]] limb_type mod_limb(limb_type divisor) const noexcept {
        std::uint64_t remainder = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) {
            remainder = (limbs_[i] + remainder * base) % divisor;
        }
        return static_cast<limb_type>(remainder);
    }

    [[nodiscard]] bool try_abs_uint64(std::uint64_t& output) const noexcept {
        if (limbs_.size() > 3) return false;
        unsigned __int128 value = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) value = value * base + limbs_[i];
        if (value > std::numeric_limits<std::uint64_t>::max()) return false;
        output = static_cast<std::uint64_t>(value);
        return true;
    }

    [[nodiscard]] bool try_int128(__int128& output) const noexcept {
        if (limbs_.size() > 4) return false;
        unsigned __int128 magnitude = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) magnitude = magnitude * base + limbs_[i];
        if (sign_ < 0) output = -static_cast<__int128>(magnitude);
        else output = static_cast<__int128>(magnitude);
        return true;
    }

    [[nodiscard]] std::uint64_t leading_two_limbs() const noexcept {
        const std::size_t n = limbs_.size();
        return static_cast<std::uint64_t>(limbs_[n - 1]) * base
             + (n >= 2 ? limbs_[n - 2] : 0U);
    }

    [[nodiscard]] Integer multiplied_by_signed(std::int64_t factor) const {
        if (factor == 0 || is_zero()) return Integer(0);
        if (factor == 1) return *this;
        if (factor == -1) return -*this;
        const bool negative = factor < 0;
        const std::uint64_t magnitude = negative
            ? std::uint64_t(0) - static_cast<std::uint64_t>(factor)
            : static_cast<std::uint64_t>(factor);
        Integer result = *this;
        if (magnitude < base) {
            const int output_sign = sign_ * (negative ? -1 : 1);
            result.sign_ = 1;
            result.mul_uint32(static_cast<limb_type>(magnitude));
            if (!result.is_zero()) result.sign_ = output_sign;
            return result;
        }
        return result * Integer(factor);
    }

    [[nodiscard]] std::string to_string() const {
        if (is_zero()) return "0";
        std::string result;
        result.reserve(limbs_.size() * base_digits + 1);
        if (sign_ < 0) result.push_back('-');
        result += std::to_string(limbs_.back());
        char buffer[base_digits + 1];
        for (std::size_t i = limbs_.size() - 1; i-- > 0;) {
            auto value = limbs_[i];
            for (int pos = base_digits - 1; pos >= 0; --pos) {
                buffer[pos] = static_cast<char>('0' + value % 10U);
                value /= 10U;
            }
            buffer[base_digits] = '\0';
            result.append(buffer, base_digits);
        }
        return result;
    }

    [[nodiscard]] long double to_long_double() const noexcept {
        long double value = 0.0L;
        for (std::size_t i = limbs_.size(); i-- > 0;) {
            value = value * static_cast<long double>(base) + static_cast<long double>(limbs_[i]);
        }
        return sign_ < 0 ? -value : value;
    }

    [[nodiscard]] long double leading_long_double(std::size_t max_limbs = 4) const noexcept {
        if (is_zero() || max_limbs == 0) return 0.0L;
        const std::size_t begin = limbs_.size() > max_limbs ? limbs_.size() - max_limbs : 0;
        long double value = 0.0L;
        for (std::size_t i = limbs_.size(); i-- > begin;) {
            value = value * static_cast<long double>(base) + static_cast<long double>(limbs_[i]);
        }
        return sign_ < 0 ? -value : value;
    }

    [[nodiscard]] bool fits_uint64() const noexcept {
        unsigned __int128 value = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) {
            value = value * base + limbs_[i];
            if (value > std::numeric_limits<std::uint64_t>::max()) return false;
        }
        return sign_ >= 0;
    }

    [[nodiscard]] std::uint64_t to_uint64() const {
        if (!fits_uint64()) throw std::overflow_error("Integer does not fit uint64_t");
        std::uint64_t value = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) value = value * base + limbs_[i];
        return value;
    }

    [[nodiscard]] std::size_t hash_value() const noexcept {
        std::size_t seed = static_cast<std::size_t>(sign_ + 1);
        for (auto limb : limbs_) {
            seed ^= std::hash<limb_type>{}(limb) + static_cast<std::size_t>(0x9e3779b97f4a7c15ULL)
                    + (seed << 6U) + (seed >> 2U);
        }
        return seed;
    }

    // Compares |a*b| and |c*d|. Leading-limb intervals usually settle the
    // result without materializing either product; exact multiplication is the fallback.
    [[nodiscard]] static int compare_abs_products(const Integer& a, const Integer& b,
                                                  const Integer& c, const Integer& d) {
        if (a.is_zero() || b.is_zero()) return (c.is_zero() || d.is_zero()) ? 0 : -1;
        if (c.is_zero() || d.is_zero()) return 1;

        struct Interval {
            unsigned __int128 lower;
            unsigned __int128 upper;
            std::size_t exponent;
        };
        const auto make_interval = [](const Integer& x, const Integer& y) -> Interval {
            const std::size_t used_x = std::min<std::size_t>(x.limbs_.size(), 2U);
            const std::size_t used_y = std::min<std::size_t>(y.limbs_.size(), 2U);
            const std::uint64_t top_x = x.limbs_.size() >= 2
                ? static_cast<std::uint64_t>(x.limbs_.back()) * base + x.limbs_[x.limbs_.size() - 2]
                : x.limbs_.back();
            const std::uint64_t top_y = y.limbs_.size() >= 2
                ? static_cast<std::uint64_t>(y.limbs_.back()) * base + y.limbs_[y.limbs_.size() - 2]
                : y.limbs_.back();
            const bool exact_x = x.limbs_.size() <= 2;
            const bool exact_y = y.limbs_.size() <= 2;
            const unsigned __int128 lower = static_cast<unsigned __int128>(top_x) * top_y;
            unsigned __int128 upper;
            if (exact_x && exact_y) {
                upper = lower + 1U;
            } else {
                upper = static_cast<unsigned __int128>(top_x + (exact_x ? 0U : 1U))
                      * static_cast<unsigned __int128>(top_y + (exact_y ? 0U : 1U));
            }
            return {lower, upper,
                    (x.limbs_.size() - used_x) + (y.limbs_.size() - used_y)};
        };
        const auto base_digits_u128 = [](unsigned __int128 value) noexcept {
            std::size_t digits = 1;
            while (value >= base) {
                value /= base;
                ++digits;
            }
            return digits;
        };
        const auto compare_scaled = [&](unsigned __int128 x, std::size_t ex,
                                        unsigned __int128 y, std::size_t ey) noexcept {
            const std::size_t total_x = base_digits_u128(x) + ex;
            const std::size_t total_y = base_digits_u128(y) + ey;
            if (total_x != total_y) return total_x < total_y ? -1 : 1;
            if (ex > ey) {
                unsigned __int128 scale = 1;
                for (std::size_t i = 0; i < ex - ey; ++i) scale *= base;
                const unsigned __int128 quotient = y / scale;
                if (x != quotient) return x < quotient ? -1 : 1;
                return y % scale == 0 ? 0 : -1;
            }
            if (ey > ex) {
                unsigned __int128 scale = 1;
                for (std::size_t i = 0; i < ey - ex; ++i) scale *= base;
                const unsigned __int128 quotient = x / scale;
                if (quotient != y) return quotient < y ? -1 : 1;
                return x % scale == 0 ? 0 : 1;
            }
            return x < y ? -1 : (x > y ? 1 : 0);
        };

        const Interval lhs = make_interval(a, b);
        const Interval rhs = make_interval(c, d);
        if (compare_scaled(lhs.upper, lhs.exponent, rhs.lower, rhs.exponent) <= 0) return -1;
        if (compare_scaled(lhs.lower, lhs.exponent, rhs.upper, rhs.exponent) >= 0) return 1;
        return compare_abs(multiply(a.abs(), b.abs()), multiply(c.abs(), d.abs()));
    }

    Integer operator+() const { return *this; }

    Integer operator-() const {
        Integer result = *this;
        result.sign_ = -result.sign_;
        return result;
    }

    [[nodiscard]] Integer abs() const {
        Integer result = *this;
        if (result.sign_ < 0) result.sign_ = 1;
        return result;
    }

    Integer& operator+=(const Integer& other) {
        if (other.is_zero()) return *this;
        if (is_zero()) {
            *this = other;
            return *this;
        }
        if (sign_ == other.sign_) {
            add_abs(other);
            return *this;
        }
        const int cmp = compare_abs(*this, other);
        if (cmp == 0) {
            clear();
        } else if (cmp > 0) {
            sub_abs(other);
        } else {
            Integer temp = other;
            temp.sub_abs(*this);
            *this = std::move(temp);
        }
        return *this;
    }

    Integer& operator-=(const Integer& other) {
        if (other.is_zero()) return *this;
        if (is_zero()) {
            *this = -other;
            return *this;
        }
        if (sign_ != other.sign_) {
            add_abs(other);
            return *this;
        }
        const int cmp = compare_abs(*this, other);
        if (cmp == 0) {
            clear();
        } else if (cmp > 0) {
            sub_abs(other);
        } else {
            Integer temp = other;
            temp.sub_abs(*this);
            temp.sign_ = -sign_;
            *this = std::move(temp);
        }
        return *this;
    }

    Integer& operator*=(const Integer& other) {
        *this = multiply(*this, other);
        return *this;
    }

    Integer& operator/=(const Integer& other) {
        *this = divmod(*this, other).first;
        return *this;
    }

    Integer& operator%=(const Integer& other) {
        *this = divmod(*this, other, false).second;
        return *this;
    }

    Integer& operator++() {
        *this += 1;
        return *this;
    }

    Integer operator++(int) {
        Integer copy = *this;
        ++*this;
        return copy;
    }

    Integer& operator--() {
        *this -= 1;
        return *this;
    }

    Integer operator--(int) {
        Integer copy = *this;
        --*this;
        return copy;
    }

    friend Integer operator+(Integer lhs, const Integer& rhs) { return lhs += rhs; }
    friend Integer operator-(Integer lhs, const Integer& rhs) { return lhs -= rhs; }
    friend Integer operator*(const Integer& lhs, const Integer& rhs) { return multiply(lhs, rhs); }
    friend Integer operator/(const Integer& lhs, const Integer& rhs) { return divmod(lhs, rhs).first; }
    friend Integer operator%(const Integer& lhs, const Integer& rhs) { return divmod(lhs, rhs).second; }

    friend bool operator==(const Integer& lhs, const Integer& rhs) noexcept {
        return lhs.sign_ == rhs.sign_ && lhs.limbs_ == rhs.limbs_;
    }

    friend bool operator!=(const Integer& lhs, const Integer& rhs) noexcept { return !(lhs == rhs); }

    friend bool operator<(const Integer& lhs, const Integer& rhs) noexcept {
        if (lhs.sign_ != rhs.sign_) return lhs.sign_ < rhs.sign_;
        if (lhs.sign_ == 0) return false;
        const int cmp = compare_abs(lhs, rhs);
        return lhs.sign_ > 0 ? cmp < 0 : cmp > 0;
    }

    friend bool operator>(const Integer& lhs, const Integer& rhs) noexcept { return rhs < lhs; }
    friend bool operator<=(const Integer& lhs, const Integer& rhs) noexcept { return !(rhs < lhs); }
    friend bool operator>=(const Integer& lhs, const Integer& rhs) noexcept { return !(lhs < rhs); }

    static std::pair<Integer, Integer> divmod(const Integer& dividend, const Integer& divisor,
                                                  bool quotient_required = true) {
        if (divisor.is_zero()) throw std::domain_error("Integer division by zero");
        if (dividend.is_zero()) return {Integer(0), Integer(0)};
        if (divisor.is_one()) return {quotient_required ? dividend : Integer(0), Integer(0)};
        if (divisor.is_minus_one()) return {quotient_required ? -dividend : Integer(0), Integer(0)};

        Integer a = dividend.abs();
        Integer b = divisor.abs();
        if (compare_abs(a, b) < 0) return {Integer(0), dividend};

        if (b.limbs_.size() == 1) {
            const auto divisor_limb = b.limbs_[0];
            Integer q;
            limb_type remainder;
            if (quotient_required) {
                q = a;
                remainder = q.div_uint32(divisor_limb);
                q.sign_ = q.is_zero() ? 0 : dividend.sign_ * divisor.sign_;
            } else {
                remainder = a.mod_limb(divisor_limb);
            }
            Integer r(remainder);
            if (!r.is_zero()) r.sign_ = dividend.sign_;
            return {std::move(q), std::move(r)};
        }

        // Knuth-style normalized long division. This works directly on limb
        // arrays and avoids allocating a temporary Integer for every quotient digit.
        const limb_type norm = static_cast<limb_type>(base / (static_cast<std::uint64_t>(b.limbs_.back()) + 1U));
        a.mul_uint32(norm);
        b.mul_uint32(norm);
        a.limbs_.push_back(0);

        const std::size_t n = b.limbs_.size();
        const std::size_t m = a.limbs_.size() - n - 1;
        Integer quotient;
        if (quotient_required) {
            quotient.sign_ = dividend.sign_ * divisor.sign_;
            quotient.limbs_.assign(m + 1, 0);
        }

        for (std::size_t jj = m + 1; jj-- > 0;) {
            const std::size_t j = jj;
            const std::uint64_t top = static_cast<std::uint64_t>(a.limbs_[j + n]) * base
                                    + a.limbs_[j + n - 1];
            std::uint64_t estimate = top / b.limbs_[n - 1];
            std::uint64_t remainder_hat = top % b.limbs_[n - 1];

            while (estimate == base
                   || estimate * static_cast<std::uint64_t>(b.limbs_[n - 2])
                      > static_cast<std::uint64_t>(base) * remainder_hat + a.limbs_[j + n - 2]) {
                --estimate;
                remainder_hat += b.limbs_[n - 1];
                if (remainder_hat >= base) break;
            }

            std::uint64_t carry = 0;
            std::int64_t borrow = 0;
            for (std::size_t i = 0; i < n; ++i) {
                const std::uint64_t product = estimate * static_cast<std::uint64_t>(b.limbs_[i]) + carry;
                const std::uint64_t low = static_cast<std::uint64_t>(product % base);
                carry = static_cast<std::uint64_t>(product / base);
                std::int64_t current = static_cast<std::int64_t>(a.limbs_[j + i])
                                     - static_cast<std::int64_t>(low) - borrow;
                if (current < 0) {
                    current += base;
                    borrow = 1;
                } else {
                    borrow = 0;
                }
                a.limbs_[j + i] = static_cast<limb_type>(current);
            }

            std::int64_t high = static_cast<std::int64_t>(a.limbs_[j + n])
                              - static_cast<std::int64_t>(carry) - borrow;
            if (high < 0) {
                --estimate;
                a.limbs_[j + n] = static_cast<limb_type>(high + base);
                std::uint64_t add_carry = 0;
                for (std::size_t i = 0; i < n; ++i) {
                    const std::uint64_t sum = static_cast<std::uint64_t>(a.limbs_[j + i])
                                            + b.limbs_[i] + add_carry;
                    a.limbs_[j + i] = static_cast<limb_type>(sum % base);
                    add_carry = sum / base;
                }
                a.limbs_[j + n] = static_cast<limb_type>(
                    (static_cast<std::uint64_t>(a.limbs_[j + n]) + add_carry) % base);
            } else {
                a.limbs_[j + n] = static_cast<limb_type>(high);
            }
            if (quotient_required) quotient.limbs_[j] = static_cast<limb_type>(estimate);
        }

        if (quotient_required) {
            quotient.trim();
            if (!quotient.is_zero()) quotient.sign_ = dividend.sign_ * divisor.sign_;
        }

        Integer remainder;
        remainder.sign_ = 1;
        remainder.limbs_.assign(a.limbs_.begin(), a.limbs_.begin() + static_cast<std::ptrdiff_t>(n));
        remainder.trim();
        remainder.div_uint32(norm);
        if (!remainder.is_zero()) remainder.sign_ = dividend.sign_;
        return {std::move(quotient), std::move(remainder)};
    }

    Integer& divide_exact(const Integer& divisor) {
        if (divisor.is_one()) return *this;
        if (same_magnitude(divisor)) {
            sign_ = sign_ * divisor.sign_;
            limbs_.assign(1, 1U);
            return *this;
        }
        if (divisor.has_single_limb()) {
            const int divisor_sign = divisor.sign_;
            (void)div_uint32(divisor.limbs_[0]);
            if (!is_zero()) sign_ *= divisor_sign;
            return *this;
        }
        *this = divmod(*this, divisor).first;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const Integer& value) {
        return out << value.to_string();
    }

    friend std::istream& operator>>(std::istream& in, Integer& value) {
        std::string token;
        in >> token;
        if (in) {
            try {
                value.read(token);
            } catch (const std::invalid_argument&) {
                in.setstate(std::ios::failbit);
            }
        }
        return in;
    }

private:
    SmallVector<limb_type, 4> limbs_; // Little endian, base 1e9; four limbs inline.
    int sign_ = 0;
    static constexpr std::size_t karatsuba_threshold = CP_RATIONAL_KARATSUBA_THRESHOLD;

    void clear() noexcept {
        limbs_.clear();
        sign_ = 0;
    }

    void trim() noexcept {
        while (!limbs_.empty() && limbs_.back() == 0) limbs_.pop_back();
        if (limbs_.empty()) sign_ = 0;
    }

    template <class T>
    void assign_integral(T value) {
        clear();
        unsigned __int128 magnitude = 0;
        if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>) {
            sign_ = value ? 1 : 0;
            magnitude = value ? 1U : 0U;
        } else {
            using U = make_unsigned_extended_t<T>;
            if constexpr (is_extended_signed_v<T>) {
                if (value < 0) {
                    sign_ = -1;
                    const U unsigned_value = static_cast<U>(value);
                    const U unsigned_magnitude = static_cast<U>(U(0) - unsigned_value);
                    magnitude = static_cast<unsigned __int128>(unsigned_magnitude);
                } else {
                    sign_ = value == 0 ? 0 : 1;
                    magnitude = static_cast<unsigned __int128>(static_cast<U>(value));
                }
            } else {
                sign_ = value == 0 ? 0 : 1;
                magnitude = static_cast<unsigned __int128>(value);
            }
        }
        while (magnitude != 0) {
            limbs_.push_back(static_cast<limb_type>(magnitude % base));
            magnitude /= base;
        }
    }

    void read(std::string_view text) {
        clear();
        if (text.empty()) throw std::invalid_argument("empty Integer string");
        std::size_t pos = 0;
        int parsed_sign = 1;
        if (text[pos] == '+' || text[pos] == '-') {
            parsed_sign = text[pos] == '-' ? -1 : 1;
            ++pos;
        }
        if (pos == text.size()) throw std::invalid_argument("invalid Integer string");
        for (std::size_t i = pos; i < text.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
                throw std::invalid_argument("invalid Integer digit");
            }
        }
        while (pos < text.size() && text[pos] == '0') ++pos;
        if (pos == text.size()) return;
        sign_ = parsed_sign;
        for (std::size_t end = text.size(); end > pos;) {
            const std::size_t begin = end >= pos + base_digits ? end - base_digits : pos;
            limb_type chunk = 0;
            for (std::size_t i = begin; i < end; ++i) {
                chunk = static_cast<limb_type>(chunk * 10U + static_cast<unsigned>(text[i] - '0'));
            }
            limbs_.push_back(chunk);
            end = begin;
        }
        trim();
    }

    static int compare_abs(const Integer& lhs, const Integer& rhs) noexcept {
        if (lhs.limbs_.size() != rhs.limbs_.size()) {
            return lhs.limbs_.size() < rhs.limbs_.size() ? -1 : 1;
        }
        for (std::size_t i = lhs.limbs_.size(); i-- > 0;) {
            if (lhs.limbs_[i] != rhs.limbs_[i]) return lhs.limbs_[i] < rhs.limbs_[i] ? -1 : 1;
        }
        return 0;
    }

    void add_abs(const Integer& other) {
        const std::size_t n = std::max(limbs_.size(), other.limbs_.size());
        limbs_.resize(n, 0);
        std::uint64_t carry = 0;
        for (std::size_t i = 0; i < n || carry != 0; ++i) {
            if (i == limbs_.size()) limbs_.push_back(0);
            const std::uint64_t sum = static_cast<std::uint64_t>(limbs_[i])
                                    + (i < other.limbs_.size() ? other.limbs_[i] : 0U) + carry;
            limbs_[i] = static_cast<limb_type>(sum % base);
            carry = sum / base;
        }
    }

    // Precondition: |*this| >= |other|.
    void sub_abs(const Integer& other) {
        std::int64_t borrow = 0;
        for (std::size_t i = 0; i < limbs_.size(); ++i) {
            std::int64_t current = static_cast<std::int64_t>(limbs_[i])
                                 - (i < other.limbs_.size() ? other.limbs_[i] : 0U) - borrow;
            if (current < 0) {
                current += base;
                borrow = 1;
            } else {
                borrow = 0;
            }
            limbs_[i] = static_cast<limb_type>(current);
        }
        trim();
    }

    void mul_uint32(limb_type factor) {
        if (is_zero() || factor == 1U) return;
        if (factor == 0U) {
            clear();
            return;
        }
        std::uint64_t carry = 0;
        for (auto& limb : limbs_) {
            const std::uint64_t current = static_cast<std::uint64_t>(limb) * factor + carry;
            limb = static_cast<limb_type>(current % base);
            carry = static_cast<std::uint64_t>(current / base);
        }
        if (carry != 0) limbs_.push_back(static_cast<limb_type>(carry));
    }

    limb_type div_uint32(limb_type divisor) {
        if (divisor == 0U) throw std::domain_error("Integer division by zero");
        std::uint64_t remainder = 0;
        for (std::size_t i = limbs_.size(); i-- > 0;) {
            const std::uint64_t current = limbs_[i] + remainder * base;
            limbs_[i] = static_cast<limb_type>(current / divisor);
            remainder = current % divisor;
        }
        trim();
        return static_cast<limb_type>(remainder);
    }

    static Integer multiply_schoolbook(const Integer& lhs, const Integer& rhs) {
        Integer result;
        if (lhs.is_zero() || rhs.is_zero()) return result;
        result.sign_ = lhs.sign_ * rhs.sign_;
        result.limbs_.assign(lhs.limbs_.size() + rhs.limbs_.size(), 0);
        for (std::size_t i = 0; i < lhs.limbs_.size(); ++i) {
            std::uint64_t carry = 0;
            for (std::size_t j = 0; j < rhs.limbs_.size() || carry != 0; ++j) {
                const std::uint64_t current = static_cast<std::uint64_t>(result.limbs_[i + j])
                    + static_cast<std::uint64_t>(lhs.limbs_[i])
                      * (j < rhs.limbs_.size() ? rhs.limbs_[j] : 0U)
                    + carry;
                result.limbs_[i + j] = static_cast<limb_type>(current % base);
                carry = static_cast<std::uint64_t>(current / base);
            }
        }
        result.trim();
        return result;
    }

    template <class Digits>
    static std::vector<std::int64_t> convert_base(const Digits& digits,
                                                   int old_digits, int new_digits) {
        static constexpr std::int64_t powers[] = {
            1LL, 10LL, 100LL, 1'000LL, 10'000LL,
            100'000LL, 1'000'000LL, 10'000'000LL, 100'000'000LL, 1'000'000'000LL
        };
        std::vector<std::int64_t> result;
        std::int64_t current = 0;
        int current_digits = 0;
        for (auto digit : digits) {
            current += static_cast<std::int64_t>(digit) * powers[static_cast<std::size_t>(current_digits)];
            current_digits += old_digits;
            while (current_digits >= new_digits) {
                result.push_back(current % powers[static_cast<std::size_t>(new_digits)]);
                current /= powers[static_cast<std::size_t>(new_digits)];
                current_digits -= new_digits;
            }
        }
        if (current != 0) result.push_back(current);
        while (!result.empty() && result.back() == 0) result.pop_back();
        return result;
    }

    static void karatsuba_multiply(const std::int64_t* a,
                                     const std::int64_t* b,
                                     std::size_t n,
                                     std::int64_t* result,
                                     std::int64_t* scratch) {
        std::fill(result, result + 2U * n, std::int64_t(0));
        if (n <= CP_RATIONAL_KARATSUBA_BASECASE) {
            for (std::size_t i = 0; i < n; ++i) {
                for (std::size_t j = 0; j < n; ++j) result[i + j] += a[i] * b[j];
            }
            return;
        }

        const std::size_t half = n >> 1U;
        karatsuba_multiply(a, b, half, result, scratch);
        karatsuba_multiply(a + half, b + half, half, result + n, scratch);

        std::int64_t* sum_a = scratch;
        std::int64_t* sum_b = scratch + half;
        std::int64_t* middle = scratch + 2U * half;
        std::int64_t* child_scratch = scratch + 4U * half;
        for (std::size_t i = 0; i < half; ++i) {
            sum_a[i] = a[i] + a[i + half];
            sum_b[i] = b[i] + b[i + half];
        }
        karatsuba_multiply(sum_a, sum_b, half, middle, child_scratch);
        for (std::size_t i = 0; i < 2U * half; ++i) {
            middle[i] -= result[i] + result[n + i];
        }
        for (std::size_t i = 0; i < 2U * half; ++i) result[half + i] += middle[i];
    }

    static Integer multiply_karatsuba(const Integer& lhs, const Integer& rhs) {
        constexpr std::int64_t karatsuba_base = 1'000'000;
        auto a = convert_base(lhs.limbs_, 9, 6);
        auto b = convert_base(rhs.limbs_, 9, 6);
        std::size_t n = 1;
        while (n < std::max(a.size(), b.size())) n <<= 1U;
        a.resize(n, 0);
        b.resize(n, 0);
#if CP_RATIONAL_REUSE_KARATSUBA_BUFFERS
        static thread_local std::vector<std::int64_t> coefficient_buffer;
        static thread_local std::vector<std::int64_t> scratch_buffer;
        auto& coefficients = coefficient_buffer;
        auto& scratch = scratch_buffer;
#else
        std::vector<std::int64_t> coefficients;
        std::vector<std::int64_t> scratch;
#endif
        coefficients.resize(2U * n);
        scratch.resize(4U * n);
        karatsuba_multiply(a.data(), b.data(), n, coefficients.data(), scratch.data());

        std::vector<limb_type> base6;
        base6.reserve(coefficients.size() + 2);
        std::int64_t carry = 0;
        for (auto coefficient : coefficients) {
            std::int64_t current = coefficient + carry;
            limb_type digit;
            if (current >= 0) {
                digit = static_cast<limb_type>(current % karatsuba_base);
                carry = current / karatsuba_base;
            } else {
                const std::int64_t borrow = (-current + karatsuba_base - 1) / karatsuba_base;
                current += borrow * karatsuba_base;
                digit = static_cast<limb_type>(current);
                carry = -borrow;
            }
            base6.push_back(digit);
        }
        while (carry > 0) {
            base6.push_back(static_cast<limb_type>(carry % karatsuba_base));
            carry /= karatsuba_base;
        }
        while (!base6.empty() && base6.back() == 0) base6.pop_back();

        Integer result;
        result.sign_ = lhs.sign_ * rhs.sign_;
        auto converted = convert_base(base6, 6, 9);
        result.limbs_.reserve(converted.size());
        for (auto digit : converted) result.limbs_.push_back(static_cast<limb_type>(digit));
        result.trim();
        return result;
    }

    static Integer multiply(const Integer& lhs, const Integer& rhs) {
        if (lhs.is_zero() || rhs.is_zero()) return Integer(0);
        if (lhs.is_one()) return rhs;
        if (rhs.is_one()) return lhs;
        if (lhs.is_minus_one()) return -rhs;
        if (rhs.is_minus_one()) return -lhs;
        if (lhs.limbs_.size() == 1) {
            Integer result = rhs;
            const int result_sign = lhs.sign_ * rhs.sign_;
            result.sign_ = result.is_zero() ? 0 : 1;
            result.mul_uint32(lhs.limbs_[0]);
            if (!result.is_zero()) result.sign_ = result_sign;
            return result;
        }
        if (rhs.limbs_.size() == 1) {
            Integer result = lhs;
            const int result_sign = lhs.sign_ * rhs.sign_;
            result.sign_ = result.is_zero() ? 0 : 1;
            result.mul_uint32(rhs.limbs_[0]);
            if (!result.is_zero()) result.sign_ = result_sign;
            return result;
        }
        const std::size_t smaller = std::min(lhs.limbs_.size(), rhs.limbs_.size());
        const std::size_t larger = std::max(lhs.limbs_.size(), rhs.limbs_.size());
        if (smaller < karatsuba_threshold || larger > smaller * 2U) {
            return multiply_schoolbook(lhs, rhs);
        }
        return multiply_karatsuba(lhs, rhs);
    }
};

inline Integer abs(const Integer& value) { return value.abs(); }

inline unsigned __int128 abs_u128(__int128 value) noexcept {
    const unsigned __int128 bits = static_cast<unsigned __int128>(value);
    return value < 0 ? static_cast<unsigned __int128>(0) - bits : bits;
}

inline unsigned __int128 gcd_u128(unsigned __int128 a, unsigned __int128 b) noexcept {
    if ((a >> 64U) == 0 && (b >> 64U) == 0) {
        return std::gcd(static_cast<std::uint64_t>(a), static_cast<std::uint64_t>(b));
    }
    while (b != 0) {
        const unsigned __int128 remainder = a % b;
        a = b;
        b = remainder;
        if ((a >> 64U) == 0 && (b >> 64U) == 0) {
            return std::gcd(static_cast<std::uint64_t>(a), static_cast<std::uint64_t>(b));
        }
    }
    return a;
}

inline Integer gcd(const Integer& lhs, const Integer& rhs) {
    if (lhs.is_zero()) return rhs.abs();
    if (rhs.is_zero()) return lhs.abs();
    if (lhs.same_magnitude(rhs)) return lhs.abs();
    if (lhs.is_one() || lhs.is_minus_one() || rhs.is_one() || rhs.is_minus_one()) return Integer(1);

    std::uint64_t lhs64 = 0;
    std::uint64_t rhs64 = 0;
    if (lhs.try_abs_uint64(lhs64) && rhs.try_abs_uint64(rhs64)) {
        return Integer(std::gcd(lhs64, rhs64));
    }

    if (lhs.has_single_limb()) {
        const auto small = lhs.single_limb();
        return Integer(std::gcd(small, rhs.mod_limb(small)));
    }
    if (rhs.has_single_limb()) {
        const auto small = rhs.single_limb();
        return Integer(std::gcd(small, lhs.mod_limb(small)));
    }

    Integer a = lhs.abs();
    Integer b = rhs.abs();
    if (a < b) std::swap(a, b);
    while (!b.is_zero()) {
        if (b.is_one()) return b;
        if (b.has_single_limb()) {
            const auto small = b.single_limb();
            return Integer(std::gcd(small, a.mod_limb(small)));
        }

        bool applied_lehmer = false;
        if (a.limb_count() == b.limb_count() && a.limb_count() >= 3) {
            std::uint64_t x = a.leading_two_limbs();
            std::uint64_t y = b.leading_two_limbs();
            std::int64_t A = 1;
            std::int64_t B = 0;
            std::int64_t C = 0;
            std::int64_t D = 1;

            for (;;) {
                const __int128 denominator1 = static_cast<__int128>(y) + C;
                const __int128 denominator2 = static_cast<__int128>(y) + D;
                if (denominator1 <= 0 || denominator2 <= 0) break;
                const __int128 q1 = (static_cast<__int128>(x) + A) / denominator1;
                const __int128 q2 = (static_cast<__int128>(x) + B) / denominator2;
                if (q1 != q2 || q1 <= 0 || q1 > std::numeric_limits<std::int64_t>::max()) break;
                const std::int64_t q = static_cast<std::int64_t>(q1);

                const __int128 next_c = static_cast<__int128>(A) - static_cast<__int128>(q) * C;
                const __int128 next_d = static_cast<__int128>(B) - static_cast<__int128>(q) * D;
                const __int128 next_y = static_cast<__int128>(x) - static_cast<__int128>(q) * y;
                if (next_c < std::numeric_limits<std::int64_t>::min()
                    || next_c > std::numeric_limits<std::int64_t>::max()
                    || next_d < std::numeric_limits<std::int64_t>::min()
                    || next_d > std::numeric_limits<std::int64_t>::max()
                    || next_y < 0
                    || next_y > std::numeric_limits<std::uint64_t>::max()) {
                    break;
                }

                const std::int64_t next_a = C;
                const std::int64_t next_b = D;
                A = next_a;
                B = next_b;
                C = static_cast<std::int64_t>(next_c);
                D = static_cast<std::int64_t>(next_d);
                x = y;
                y = static_cast<std::uint64_t>(next_y);
            }

            if (B != 0) {
                Integer old_a = std::move(a);
                Integer old_b = std::move(b);
                a = old_a.multiplied_by_signed(A) + old_b.multiplied_by_signed(B);
                b = old_a.multiplied_by_signed(C) + old_b.multiplied_by_signed(D);
                a = a.abs();
                b = b.abs();
                if (a < b) std::swap(a, b);
                applied_lehmer = true;
            }
        }

        if (!applied_lehmer) {
            a %= b;
            std::swap(a, b);
        }
    }
    return a;
}

inline Integer lcm(const Integer& a, const Integer& b) {
    if (a.is_zero() || b.is_zero()) return Integer(0);
    return (a / gcd(a, b) * b).abs();
}

inline Integer pow(Integer base_value, std::uint64_t exponent) {
    Integer result(1);
    while (exponent != 0) {
        if (exponent & 1U) result *= base_value;
        exponent >>= 1U;
        if (exponent != 0) base_value *= base_value;
    }
    return result;
}

inline Integer exact_divide(Integer value, const Integer& divisor) {
    return value.divide_exact(divisor);
}


} // namespace detail

class Rational {
public:
    Rational() : numerator_(0), denominator_(1) {}

    // Integral conversion is intentionally allowed for contest ergonomics.
    // Floating-point construction is explicitly deleted below.
    template <class T, detail::enable_integral_t<T> = 0>
    Rational(T value) : numerator_(value), denominator_(1) {}

    template <class T, detail::enable_floating_t<T> = 0>
    Rational(T) = delete;

    template <class T, class U,
              std::enable_if_t<detail::is_extended_integral_v<T>
                            && detail::is_extended_integral_v<U>, int> = 0>
    Rational(T numerator, U denominator)
        : numerator_(numerator), denominator_(denominator) {
        normalize();
    }

    template <class T, class U,
              std::enable_if_t<std::is_floating_point_v<std::remove_cv_t<T>>
                            || std::is_floating_point_v<std::remove_cv_t<U>>, int> = 0>
    Rational(T, U) = delete;

    explicit Rational(std::string_view text) { read(text); }

    Rational(std::string_view numerator, std::string_view denominator)
        : numerator_(numerator), denominator_(denominator) {
        normalize();
    }

    [[nodiscard]] std::string numerator_string() const { return numerator_.to_string(); }
    [[nodiscard]] std::string denominator_string() const { return denominator_.to_string(); }
    [[nodiscard]] int sign() const noexcept { return numerator_.sign(); }
    [[nodiscard]] bool is_zero() const noexcept { return numerator_.is_zero(); }
    [[nodiscard]] bool is_integer() const noexcept { return denominator_.is_one(); }
    explicit operator bool() const noexcept { return !is_zero(); }

    [[nodiscard]] std::string to_string() const {
        if (denominator_.is_one()) return numerator_.to_string();
        return numerator_.to_string() + "/" + denominator_.to_string();
    }

    // Truncates after `digits` decimal places; it does not round.
    [[nodiscard]] std::string to_decimal(std::size_t digits,
                                         bool trim_trailing_zeros = false) const {
        if (is_zero()) return "0";
        detail::Integer magnitude = numerator_.abs();
        auto qr = detail::Integer::divmod(magnitude, denominator_);
        std::string result;
        if (numerator_.is_negative()) result.push_back('-');
        result += qr.first.to_string();
        if (digits == 0) return result;
        result.push_back('.');
        detail::Integer remainder = std::move(qr.second);
        std::size_t remaining = digits;
        while (remaining != 0) {
            const std::size_t chunk_digits = std::min<std::size_t>(remaining, 9U);
            std::uint32_t scale = 1;
            for (std::size_t i = 0; i < chunk_digits; ++i) scale *= 10U;
            remainder *= scale;
            auto chunk_qr = detail::Integer::divmod(remainder, denominator_);
            std::uint32_t chunk = static_cast<std::uint32_t>(chunk_qr.first.to_uint64());
            char buffer[9];
            for (std::size_t i = chunk_digits; i-- > 0;) {
                buffer[i] = static_cast<char>('0' + chunk % 10U);
                chunk /= 10U;
            }
            result.append(buffer, chunk_digits);
            remainder = std::move(chunk_qr.second);
            remaining -= chunk_digits;
        }
        if (trim_trailing_zeros) {
            while (!result.empty() && result.back() == '0') result.pop_back();
            if (!result.empty() && result.back() == '.') result.pop_back();
        }
        return result;
    }

    // Explicit, named conversion only. No implicit floating-point conversion exists.
    [[nodiscard]] long double to_long_double() const noexcept {
        if (is_zero()) return 0.0L;
        constexpr std::size_t sampled_limbs = 4;
        const std::size_t numerator_used = std::min(numerator_.limb_count(), sampled_limbs);
        const std::size_t denominator_used = std::min(denominator_.limb_count(), sampled_limbs);
        const long double exponent =
            static_cast<long double>(numerator_.limb_count() - numerator_used)
          - static_cast<long double>(denominator_.limb_count() - denominator_used);
        return (numerator_.leading_long_double(sampled_limbs)
                / denominator_.leading_long_double(sampled_limbs))
             * std::pow(static_cast<long double>(detail::Integer::base), exponent);
    }

    [[nodiscard]] std::size_t hash_value() const noexcept {
        std::size_t seed = numerator_.hash_value();
        seed ^= denominator_.hash_value() + static_cast<std::size_t>(0x9e3779b97f4a7c15ULL)
                + (seed << 6U) + (seed >> 2U);
        return seed;
    }

    Rational operator+() const { return *this; }
    Rational operator-() const { return Rational(-numerator_, denominator_, normalized_tag{}); }

    [[nodiscard]] Rational abs() const {
        return Rational(numerator_.abs(), denominator_, normalized_tag{});
    }

    [[nodiscard]] Rational reciprocal() const {
        if (is_zero()) throw std::domain_error("reciprocal of zero");
        if (numerator_.is_negative()) {
            return Rational(-denominator_, -numerator_, normalized_tag{});
        }
        return Rational(denominator_, numerator_, normalized_tag{});
    }

    Rational& operator+=(const Rational& other) { return add_assign(other, false); }

    Rational& operator-=(const Rational& other) { return add_assign(other, true); }

    Rational& operator*=(const Rational& other) {
        if (is_zero() || other.is_zero()) {
            numerator_ = detail::Integer(0);
            denominator_ = 1;
            return *this;
        }
        {
            __int128 an, ad, bn, bd;
            if (numerator_.try_int128(an) && denominator_.try_int128(ad)
                && other.numerator_.try_int128(bn) && other.denominator_.try_int128(bd)) {
                const unsigned __int128 g1 = detail::gcd_u128(detail::abs_u128(an), static_cast<unsigned __int128>(bd));
                const unsigned __int128 g2 = detail::gcd_u128(detail::abs_u128(bn), static_cast<unsigned __int128>(ad));
                an /= static_cast<__int128>(g1);
                bd /= static_cast<__int128>(g1);
                bn /= static_cast<__int128>(g2);
                ad /= static_cast<__int128>(g2);
                __int128 product_numerator;
                __int128 product_denominator;
                if (!__builtin_mul_overflow(an, bn, &product_numerator)
                    && !__builtin_mul_overflow(ad, bd, &product_denominator)) {
                    numerator_ = product_numerator;
                    denominator_ = product_denominator;
                    return *this;
                }
            }
        }
        if (other.denominator_.is_one()) return multiply_integer(other.numerator_);
        if (denominator_.is_one()) {
            detail::Integer factor = numerator_;
            *this = other;
            return multiply_integer(std::move(factor));
        }
        const detail::Integer left_cancel = detail::gcd(numerator_.abs(), other.denominator_);
        const detail::Integer right_cancel = detail::gcd(other.numerator_.abs(), denominator_);
        numerator_ = detail::exact_divide(numerator_, left_cancel)
                   * detail::exact_divide(other.numerator_, right_cancel);
        denominator_ = detail::exact_divide(denominator_, right_cancel)
                     * detail::exact_divide(other.denominator_, left_cancel);
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        if (other.is_zero()) throw std::domain_error("Rational division by zero");
        if (is_zero()) return *this;
        {
            __int128 an, ad, bn, bd;
            if (numerator_.try_int128(an) && denominator_.try_int128(ad)
                && other.numerator_.try_int128(bn) && other.denominator_.try_int128(bd)) {
                const unsigned __int128 g1 = detail::gcd_u128(detail::abs_u128(an), detail::abs_u128(bn));
                const unsigned __int128 g2 = detail::gcd_u128(static_cast<unsigned __int128>(ad), static_cast<unsigned __int128>(bd));
                an /= static_cast<__int128>(g1);
                bn /= static_cast<__int128>(g1);
                ad /= static_cast<__int128>(g2);
                bd /= static_cast<__int128>(g2);
                __int128 quotient_numerator;
                __int128 quotient_denominator;
                if (!__builtin_mul_overflow(an, bd, &quotient_numerator)
                    && !__builtin_mul_overflow(ad, bn, &quotient_denominator)) {
                    if (quotient_denominator < 0) {
                        if (quotient_denominator == std::numeric_limits<__int128>::min()
                            || quotient_numerator == std::numeric_limits<__int128>::min()) {
                            // Fall through to the arbitrary-precision path.
                        } else {
                            quotient_numerator = -quotient_numerator;
                            quotient_denominator = -quotient_denominator;
                            numerator_ = quotient_numerator;
                            denominator_ = quotient_denominator;
                            return *this;
                        }
                    } else {
                        numerator_ = quotient_numerator;
                        denominator_ = quotient_denominator;
                        return *this;
                    }
                }
            }
        }
        const detail::Integer left_cancel = detail::gcd(numerator_.abs(), other.numerator_.abs());
        const detail::Integer right_cancel = detail::gcd(denominator_, other.denominator_);
        detail::Integer new_numerator = detail::exact_divide(numerator_, left_cancel)
                                     * detail::exact_divide(other.denominator_, right_cancel);
        detail::Integer new_denominator = detail::exact_divide(denominator_, right_cancel)
                                       * detail::exact_divide(other.numerator_, left_cancel);
        if (new_denominator.is_negative()) {
            new_numerator = -new_numerator;
            new_denominator = -new_denominator;
        }
        numerator_ = std::move(new_numerator);
        denominator_ = std::move(new_denominator);
        return *this;
    }

    // C++-style remainder: a % b == a - trunc(a / b) * b.
    Rational& operator%=(const Rational& other) {
        if (other.is_zero()) throw std::domain_error("Rational remainder by zero");
        __int128 an, ad, bn, bd;
        if (numerator_.try_int128(an) && denominator_.try_int128(ad)
            && other.numerator_.try_int128(bn) && other.denominator_.try_int128(bd)) {
            __int128 dividend;
            __int128 divisor;
            __int128 result_denominator;
            if (!__builtin_mul_overflow(an, bd, &dividend)
                && !__builtin_mul_overflow(ad, bn, &divisor)
                && !__builtin_mul_overflow(ad, bd, &result_denominator)
                && !(dividend == std::numeric_limits<__int128>::min() && divisor == -1)) {
                assign_normalized_i128(dividend % divisor, result_denominator);
                return *this;
            }
        }
        *this -= (*this / other).trunc() * other;
        return *this;
    }

    // Direct integral operations avoid constructing a temporary Rational.
    template <class T, detail::enable_integral_t<T> = 0>
    Rational& operator+=(T value) {
        detail::Integer integer(value);
        __int128 n, d, v;
        if (numerator_.try_int128(n) && denominator_.try_int128(d) && integer.try_int128(v)) {
            __int128 term;
            __int128 result;
            if (!__builtin_mul_overflow(d, v, &term)
                && !__builtin_add_overflow(n, term, &result)) {
                numerator_ = result;
                return *this;
            }
        }
        numerator_ += denominator_ * integer;
        return *this;
    }

    template <class T, detail::enable_integral_t<T> = 0>
    Rational& operator-=(T value) {
        detail::Integer integer(value);
        __int128 n, d, v;
        if (numerator_.try_int128(n) && denominator_.try_int128(d) && integer.try_int128(v)) {
            __int128 term;
            __int128 result;
            if (!__builtin_mul_overflow(d, v, &term)
                && !__builtin_sub_overflow(n, term, &result)) {
                numerator_ = result;
                return *this;
            }
        }
        numerator_ -= denominator_ * integer;
        return *this;
    }

    template <class T, detail::enable_integral_t<T> = 0>
    Rational& operator*=(T value) {
        return multiply_integer(detail::Integer(value));
    }

    template <class T, detail::enable_integral_t<T> = 0>
    Rational& operator/=(T value) {
        return divide_integer(detail::Integer(value));
    }

    template <class T, detail::enable_integral_t<T> = 0>
    Rational& operator%=(T value) {
        detail::Integer integer(value);
        if (integer.is_zero()) throw std::domain_error("Rational remainder by zero integer");
        __int128 n, d, v;
        if (numerator_.try_int128(n) && denominator_.try_int128(d) && integer.try_int128(v)) {
            __int128 divisor;
            if (!__builtin_mul_overflow(d, v, &divisor)
                && !(n == std::numeric_limits<__int128>::min() && divisor == -1)) {
                const __int128 remainder = n % divisor;
                if (remainder == 0) {
                    numerator_ = 0;
                    denominator_ = 1;
                } else {
                    numerator_ = remainder;
                }
                return *this;
            }
        }
        *this -= (*this / value).trunc() * value;
        return *this;
    }

    Rational& operator++() { return *this += 1; }
    Rational operator++(int) {
        Rational copy = *this;
        ++*this;
        return copy;
    }
    Rational& operator--() { return *this -= 1; }
    Rational operator--(int) {
        Rational copy = *this;
        --*this;
        return copy;
    }

    friend Rational operator+(Rational lhs, const Rational& rhs) { return lhs += rhs; }
    friend Rational operator-(Rational lhs, const Rational& rhs) { return lhs -= rhs; }
    friend Rational operator*(Rational lhs, const Rational& rhs) { return lhs *= rhs; }
    friend Rational operator/(Rational lhs, const Rational& rhs) { return lhs /= rhs; }
    friend Rational operator%(Rational lhs, const Rational& rhs) { return lhs %= rhs; }

    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator+(Rational lhs, T rhs) { return lhs += rhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator+(T lhs, Rational rhs) { return rhs += lhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator-(Rational lhs, T rhs) { return lhs -= rhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator-(T lhs, const Rational& rhs) {
        Rational result(lhs);
        return result -= rhs;
    }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator*(Rational lhs, T rhs) { return lhs *= rhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator*(T lhs, Rational rhs) { return rhs *= lhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator/(Rational lhs, T rhs) { return lhs /= rhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator/(T lhs, const Rational& rhs) {
        Rational result(lhs);
        return result /= rhs;
    }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator%(Rational lhs, T rhs) { return lhs %= rhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend Rational operator%(T lhs, const Rational& rhs) {
        Rational result(lhs);
        return result %= rhs;
    }

    friend bool operator==(const Rational& lhs, const Rational& rhs) noexcept {
        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }
    friend bool operator!=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs == rhs); }

    friend bool operator<(const Rational& lhs, const Rational& rhs) {
        const int lhs_sign = lhs.numerator_.sign();
        const int rhs_sign = rhs.numerator_.sign();
        if (lhs_sign != rhs_sign) return lhs_sign < rhs_sign;
        if (lhs_sign == 0) return false;
        if (lhs.denominator_ == rhs.denominator_) return lhs.numerator_ < rhs.numerator_;
        const int comparison = detail::Integer::compare_abs_products(
            lhs.numerator_, rhs.denominator_, rhs.numerator_, lhs.denominator_);
        return lhs_sign > 0 ? comparison < 0 : comparison > 0;
    }
    friend bool operator>(const Rational& lhs, const Rational& rhs) { return rhs < lhs; }
    friend bool operator<=(const Rational& lhs, const Rational& rhs) { return !(rhs < lhs); }
    friend bool operator>=(const Rational& lhs, const Rational& rhs) { return !(lhs < rhs); }

    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator==(const Rational& lhs, T rhs) { return lhs.compare_integer(rhs) == 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator==(T lhs, const Rational& rhs) { return rhs.compare_integer(lhs) == 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator!=(const Rational& lhs, T rhs) { return lhs.compare_integer(rhs) != 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator!=(T lhs, const Rational& rhs) { return rhs.compare_integer(lhs) != 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator<(const Rational& lhs, T rhs) { return lhs.compare_integer(rhs) < 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator<(T lhs, const Rational& rhs) { return rhs.compare_integer(lhs) > 0; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator>(const Rational& lhs, T rhs) { return rhs < lhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator>(T lhs, const Rational& rhs) { return rhs < lhs; }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator<=(const Rational& lhs, T rhs) { return !(rhs < lhs); }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator<=(T lhs, const Rational& rhs) { return !(rhs < lhs); }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator>=(const Rational& lhs, T rhs) { return !(lhs < rhs); }
    template <class T, detail::enable_integral_t<T> = 0>
    friend bool operator>=(T lhs, const Rational& rhs) { return !(lhs < rhs); }

    // Integer-valued results are Rational objects with denominator 1.
    [[nodiscard]] Rational trunc() const {
        return Rational(numerator_ / denominator_, detail::Integer(1), normalized_tag{});
    }

    [[nodiscard]] Rational floor() const {
        auto qr = detail::Integer::divmod(numerator_, denominator_);
        if (numerator_.is_negative() && !qr.second.is_zero()) --qr.first;
        return Rational(std::move(qr.first), detail::Integer(1), normalized_tag{});
    }

    [[nodiscard]] Rational ceil() const {
        auto qr = detail::Integer::divmod(numerator_, denominator_);
        if (numerator_.is_positive() && !qr.second.is_zero()) ++qr.first;
        return Rational(std::move(qr.first), detail::Integer(1), normalized_tag{});
    }

    // Nearest integer; exact halves are rounded away from zero.
    [[nodiscard]] Rational round() const {
        auto qr = detail::Integer::divmod(numerator_, denominator_);
        if ((qr.second.abs() * 2) >= denominator_) {
            qr.first += numerator_.is_negative() ? detail::Integer(-1) : detail::Integer(1);
        }
        return Rational(std::move(qr.first), detail::Integer(1), normalized_tag{});
    }

    // Nearest integer; exact halves are rounded to an even integer.
    [[nodiscard]] Rational round_ties_to_even() const {
        auto qr = detail::Integer::divmod(numerator_, denominator_);
        const detail::Integer twice = qr.second.abs() * 2;
        if (twice > denominator_) {
            qr.first += numerator_.is_negative() ? detail::Integer(-1) : detail::Integer(1);
        } else if (twice == denominator_) {
            const detail::Integer parity = qr.first.abs() % 2;
            if (!parity.is_zero()) {
                qr.first += numerator_.is_negative() ? detail::Integer(-1) : detail::Integer(1);
            }
        }
        return Rational(std::move(qr.first), detail::Integer(1), normalized_tag{});
    }

    // Mathematical fractional part in [0, 1).
    [[nodiscard]] Rational fractional_part() const { return *this - floor(); }

    // Fractional part with the sign of the value, matching truncation.
    [[nodiscard]] Rational trunc_fractional_part() const { return *this - trunc(); }

    [[nodiscard]] Rational pow(std::int64_t exponent) const {
        if (exponent == 0) return Rational(1);
        const bool negative_exponent = exponent < 0;
        const std::uint64_t magnitude = negative_exponent
            ? std::uint64_t(0) - static_cast<std::uint64_t>(exponent)
            : static_cast<std::uint64_t>(exponent);
        if (negative_exponent && is_zero()) {
            throw std::domain_error("zero cannot have a negative exponent");
        }
        Rational result(detail::pow(numerator_, magnitude),
                        detail::pow(denominator_, magnitude), normalized_tag{});
        return negative_exponent ? result.reciprocal() : result;
    }

    // Continued-fraction terms are returned as integer-valued Rational objects,
    // keeping the internal multiprecision integer type out of the public API.
    [[nodiscard]] std::vector<Rational> continued_fraction() const {
        std::vector<Rational> result;
        detail::Integer n = numerator_;
        detail::Integer d = denominator_;
        while (!d.is_zero()) {
            auto qr = detail::Integer::divmod(n, d);
            if (qr.second.is_negative()) {
                --qr.first;
                qr.second += d;
            }
            result.push_back(Rational(std::move(qr.first), detail::Integer(1), normalized_tag{}));
            n = std::move(d);
            d = std::move(qr.second);
        }
        return result;
    }

    static Rational from_continued_fraction(const std::vector<Rational>& terms) {
        if (terms.empty()) throw std::invalid_argument("empty continued fraction");
        for (const Rational& term : terms) {
            if (!term.is_integer()) {
                throw std::invalid_argument("continued-fraction term is not an integer");
            }
        }
        detail::Integer numerator = terms.back().numerator_;
        detail::Integer denominator = 1;
        for (std::size_t i = terms.size() - 1; i-- > 0;) {
            std::swap(numerator, denominator);
            numerator += terms[i].numerator_ * denominator;
        }
        Rational result;
        result.numerator_ = std::move(numerator);
        result.denominator_ = std::move(denominator);
        result.normalize();
        return result;
    }

    static Rational mediant(const Rational& lhs, const Rational& rhs) {
        Rational result;
        result.numerator_ = lhs.numerator_ + rhs.numerator_;
        result.denominator_ = lhs.denominator_ + rhs.denominator_;
        result.normalize();
        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const Rational& value) {
        return out << value.to_string();
    }

    friend std::istream& operator>>(std::istream& in, Rational& value) {
        std::string token;
        in >> token;
        if (in) {
            try {
                value.read(token);
            } catch (const std::exception&) {
                in.setstate(std::ios::failbit);
            }
        }
        return in;
    }

private:
    struct normalized_tag {};
    detail::Integer numerator_;
    detail::Integer denominator_;

    Rational(detail::Integer numerator, detail::Integer denominator, normalized_tag)
        : numerator_(std::move(numerator)), denominator_(std::move(denominator)) {}

    template <class T>
    [[nodiscard]] int compare_integer(T value) const {
        detail::Integer integer(value);
        if (denominator_.is_one()) {
            return numerator_ < integer ? -1 : (integer < numerator_ ? 1 : 0);
        }
        const int lhs_sign = numerator_.sign();
        const int rhs_sign = integer.sign();
        if (lhs_sign != rhs_sign) return lhs_sign < rhs_sign ? -1 : 1;
        if (lhs_sign == 0) return 0;

        __int128 n, d, v;
        if (numerator_.try_int128(n) && denominator_.try_int128(d) && integer.try_int128(v)) {
            __int128 scaled;
            if (!__builtin_mul_overflow(v, d, &scaled)) {
                return n < scaled ? -1 : (n > scaled ? 1 : 0);
            }
        }
        const detail::Integer one(1);
        const int magnitude_comparison = detail::Integer::compare_abs_products(
            numerator_, one, integer, denominator_);
        return lhs_sign > 0 ? magnitude_comparison : -magnitude_comparison;
    }

    void assign_normalized_i128(__int128 numerator, __int128 denominator) {
        if (numerator == 0) {
            numerator_ = 0;
            denominator_ = 1;
            return;
        }
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        const unsigned __int128 common = detail::gcd_u128(
            detail::abs_u128(numerator), static_cast<unsigned __int128>(denominator));
        numerator_ = numerator / static_cast<__int128>(common);
        denominator_ = denominator / static_cast<__int128>(common);
    }

    Rational& add_assign(const Rational& other, bool subtract) {
        if (other.is_zero()) return *this;
        if (is_zero()) {
            *this = other;
            if (subtract) numerator_ = -numerator_;
            return *this;
        }
        {
            __int128 an, ad, bn, bd;
            if (numerator_.try_int128(an) && denominator_.try_int128(ad)
                && other.numerator_.try_int128(bn) && other.denominator_.try_int128(bd)) {
                __int128 left;
                __int128 right;
                __int128 combined;
                __int128 product_denominator;
                if (!__builtin_mul_overflow(an, bd, &left)
                    && !__builtin_mul_overflow(bn, ad, &right)
                    && !(subtract ? __builtin_sub_overflow(left, right, &combined)
                                  : __builtin_add_overflow(left, right, &combined))
                    && !__builtin_mul_overflow(ad, bd, &product_denominator)) {
                    assign_normalized_i128(combined, product_denominator);
                    return *this;
                }
            }
        }
        if (other.denominator_.is_one()) {
            const detail::Integer term = denominator_ * other.numerator_;
            if (subtract) numerator_ -= term;
            else numerator_ += term;
            return *this;
        }
        if (denominator_.is_one()) {
            numerator_ *= other.denominator_;
            if (subtract) numerator_ -= other.numerator_;
            else numerator_ += other.numerator_;
            denominator_ = other.denominator_;
            return *this;
        }
        if (denominator_ == other.denominator_) {
            if (subtract) numerator_ -= other.numerator_;
            else numerator_ += other.numerator_;
            if (numerator_.is_zero()) {
                denominator_ = 1;
                return *this;
            }
            const detail::Integer common = detail::gcd(numerator_.abs(), denominator_);
            if (!common.is_one()) {
                numerator_ = detail::exact_divide(std::move(numerator_), common);
                denominator_ = detail::exact_divide(std::move(denominator_), common);
            }
            return *this;
        }
        const detail::Integer common = detail::gcd(denominator_, other.denominator_);
        if (common.is_one()) {
            const detail::Integer term = other.numerator_ * denominator_;
            numerator_ *= other.denominator_;
            if (subtract) numerator_ -= term;
            else numerator_ += term;
            denominator_ *= other.denominator_;
            return *this;
        }
        const detail::Integer lhs_factor = detail::exact_divide(other.denominator_, common);
        const detail::Integer rhs_factor = detail::exact_divide(denominator_, common);
        detail::Integer new_numerator = numerator_ * lhs_factor;
        const detail::Integer term = other.numerator_ * rhs_factor;
        if (subtract) new_numerator -= term;
        else new_numerator += term;
        if (new_numerator.is_zero()) {
            numerator_ = detail::Integer(0);
            denominator_ = 1;
            return *this;
        }
        const detail::Integer extra = detail::gcd(new_numerator.abs(), common);
        numerator_ = detail::exact_divide(std::move(new_numerator), extra);
        denominator_ = rhs_factor * detail::exact_divide(other.denominator_, extra);
        return *this;
    }

    Rational& multiply_integer(detail::Integer factor) {
        if (factor.is_zero() || is_zero()) {
            numerator_ = detail::Integer(0);
            denominator_ = 1;
            return *this;
        }
        if (factor.is_one()) return *this;
        if (factor.is_minus_one()) {
            numerator_ = -numerator_;
            return *this;
        }
        {
            __int128 n, d, f;
            if (numerator_.try_int128(n) && denominator_.try_int128(d) && factor.try_int128(f)) {
                const unsigned __int128 common = detail::gcd_u128(
                    detail::abs_u128(f), static_cast<unsigned __int128>(d));
                f /= static_cast<__int128>(common);
                d /= static_cast<__int128>(common);
                __int128 product;
                if (!__builtin_mul_overflow(n, f, &product)) {
                    numerator_ = product;
                    denominator_ = d;
                    return *this;
                }
            }
        }
        if (denominator_.is_one()) {
            numerator_ *= factor;
            return *this;
        }
        const detail::Integer common = detail::gcd(factor.abs(), denominator_);
        numerator_ *= detail::exact_divide(std::move(factor), common);
        if (!common.is_one()) denominator_ = detail::exact_divide(std::move(denominator_), common);
        return *this;
    }

    Rational& divide_integer(detail::Integer divisor) {
        if (divisor.is_zero()) throw std::domain_error("Rational division by zero integer");
        if (is_zero()) return *this;
        if (divisor.is_one()) return *this;
        if (divisor.is_minus_one()) {
            numerator_ = -numerator_;
            return *this;
        }
        {
            __int128 n, d, v;
            if (numerator_.try_int128(n) && denominator_.try_int128(d) && divisor.try_int128(v)) {
                const unsigned __int128 common = detail::gcd_u128(
                    detail::abs_u128(n), detail::abs_u128(v));
                n /= static_cast<__int128>(common);
                v /= static_cast<__int128>(common);
                __int128 product_denominator;
                if (!__builtin_mul_overflow(d, v, &product_denominator)) {
                    if (product_denominator < 0) {
                        if (product_denominator == std::numeric_limits<__int128>::min()
                            || n == std::numeric_limits<__int128>::min()) {
                            // Fall through to the arbitrary-precision path.
                        } else {
                            n = -n;
                            product_denominator = -product_denominator;
                            numerator_ = n;
                            denominator_ = product_denominator;
                            return *this;
                        }
                    } else {
                        numerator_ = n;
                        denominator_ = product_denominator;
                        return *this;
                    }
                }
            }
        }
        const bool negative = divisor.is_negative();
        divisor = divisor.abs();
        const detail::Integer common = detail::gcd(numerator_.abs(), divisor);
        numerator_ = detail::exact_divide(std::move(numerator_), common);
        denominator_ *= detail::exact_divide(std::move(divisor), common);
        if (negative) numerator_ = -numerator_;
        return *this;
    }

    void normalize() {
        if (denominator_.is_zero()) throw std::domain_error("Rational denominator is zero");
        __int128 small_numerator;
        __int128 small_denominator;
        if (numerator_.try_int128(small_numerator) && denominator_.try_int128(small_denominator)
            && small_numerator != std::numeric_limits<__int128>::min()
            && small_denominator != std::numeric_limits<__int128>::min()) {
            assign_normalized_i128(small_numerator, small_denominator);
            return;
        }
        if (numerator_.is_zero()) {
            denominator_ = 1;
            return;
        }
        if (denominator_.is_negative()) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const detail::Integer common = detail::gcd(numerator_.abs(), denominator_);
        if (!common.is_one()) {
            numerator_ /= common;
            denominator_ /= common;
        }
    }

    void read(std::string_view text) {
        const auto slash = text.find('/');
        if (slash == std::string_view::npos) {
            numerator_ = detail::Integer(text);
            denominator_ = 1;
            return;
        }
        if (text.find('/', slash + 1) != std::string_view::npos) {
            throw std::invalid_argument("invalid Rational string");
        }
        numerator_ = detail::Integer(text.substr(0, slash));
        denominator_ = detail::Integer(text.substr(slash + 1));
        normalize();
    }
};

inline Rational abs(const Rational& value) { return value.abs(); }
inline Rational floor(const Rational& value) { return value.floor(); }
inline Rational ceil(const Rational& value) { return value.ceil(); }
inline Rational trunc(const Rational& value) { return value.trunc(); }
inline Rational round(const Rational& value) { return value.round(); }
inline Rational pow(const Rational& value, std::int64_t exponent) { return value.pow(exponent); }

} // namespace cp

namespace std {
template <>
struct hash<cp::Rational> {
    std::size_t operator()(const cp::Rational& value) const noexcept {
        return value.hash_value();
    }
};
} // namespace std

#endif  // CPPLIB_SRC_STRUCTURE_TYPES_RATIONAL_RATIONAL_HPP_INCLUDED

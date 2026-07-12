#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace source_zip_internal{

inline constexpr char alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

template<class U>
void append_varuint(std::vector<unsigned char>& bytes, U value){
    static_assert(std::is_unsigned_v<U>);
    while(value >= U(128)){
        bytes.push_back(static_cast<unsigned char>((value & U(127)) | U(128)));
        value >>= 7;
    }
    bytes.push_back(static_cast<unsigned char>(value));
}

template<class U>
U read_varuint(const std::vector<unsigned char>& bytes, std::size_t& position){
    static_assert(std::is_unsigned_v<U>);
    U result = 0;
    int shift = 0;
    constexpr int digits = std::numeric_limits<U>::digits;
    while(true){
        if(position == bytes.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: truncated source zip data.");
        }
        const unsigned char byte = bytes[position++];
        const U part = static_cast<U>(byte & 127U);
        if(shift >= digits || (part << shift) >> shift != part)[[unlikely]]{
            throw std::runtime_error("library assertion fault: source zip varint overflow.");
        }
        result |= part << shift;
        if((byte & 128U) == 0) return result;
        shift += 7;
    }
}

inline std::string encode_base64(const std::vector<unsigned char>& bytes){
    std::string result;
    result.reserve((bytes.size() * 8 + 5) / 6);
    std::uint32_t buffer = 0;
    int bit_count = 0;
    for(unsigned char byte: bytes){
        buffer = (buffer << 8) | byte;
        bit_count += 8;
        while(bit_count >= 6){
            bit_count -= 6;
            result.push_back(alphabet[(buffer >> bit_count) & 63U]);
        }
    }
    if(bit_count != 0){
        result.push_back(alphabet[(buffer << (6 - bit_count)) & 63U]);
    }
    return result;
}

inline int base64_value(char c){
    if('A' <= c && c <= 'Z') return c - 'A';
    if('a' <= c && c <= 'z') return c - 'a' + 26;
    if('0' <= c && c <= '9') return c - '0' + 52;
    if(c == '-') return 62;
    if(c == '_') return 63;
    return -1;
}

inline std::vector<unsigned char> decode_base64(std::string_view text){
    if(text.size() % 4 == 1)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid source zip base64 length.");
    }
    std::vector<unsigned char> bytes;
    bytes.reserve(text.size() * 6 / 8);
    std::uint32_t buffer = 0;
    int bit_count = 0;
    for(char c: text){
        const int value = base64_value(c);
        if(value < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid source zip character.");
        }
        buffer = (buffer << 6) | static_cast<std::uint32_t>(value);
        bit_count += 6;
        if(bit_count >= 8){
            bit_count -= 8;
            bytes.push_back(static_cast<unsigned char>((buffer >> bit_count) & 255U));
        }
    }
    if(bit_count != 0 && (buffer & ((std::uint32_t(1) << bit_count) - 1)) != 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: nonzero source zip padding.");
    }
    return bytes;
}

template<class T>
std::make_unsigned_t<T> encode_integer(T value){
    using U = std::make_unsigned_t<T>;
    if constexpr(std::is_signed_v<T>){
        if(value >= 0) return static_cast<U>(value) << 1;
        const U magnitude = static_cast<U>(-(value + 1)) + 1;
        return (magnitude << 1) - 1;
    }else{
        return value;
    }
}

template<class T>
T decode_integer(std::make_unsigned_t<T> encoded){
    using U = std::make_unsigned_t<T>;
    if constexpr(!std::is_signed_v<T>){
        return encoded;
    }else if((encoded & U(1)) == 0){
        const U value = encoded >> 1;
        if(value > static_cast<U>(std::numeric_limits<T>::max()))[[unlikely]]{
            throw std::runtime_error("library assertion fault: source zip integer overflow.");
        }
        return static_cast<T>(value);
    }else{
        const U magnitude = (encoded >> 1) + 1;
        const U minimum_magnitude = static_cast<U>(std::numeric_limits<T>::max()) + 1;
        if(magnitude > minimum_magnitude)[[unlikely]]{
            throw std::runtime_error("library assertion fault: source zip integer overflow.");
        }
        if(magnitude == minimum_magnitude) return std::numeric_limits<T>::min();
        return static_cast<T>(-static_cast<T>(magnitude));
    }
}

} // namespace source_zip_internal

template<class T>
std::string source_zip(const std::vector<T>& values){
    static_assert(std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>);
    using U = std::make_unsigned_t<T>;
    std::vector<unsigned char> bytes;
    bytes.reserve(values.size() + 16);
    source_zip_internal::append_varuint(
        bytes,
        static_cast<std::uint64_t>(values.size())
    );
    for(std::size_t left = 0; left < values.size();){
        std::size_t right = left + 1;
        while(right < values.size() && values[right] == values[left]) ++right;
        source_zip_internal::append_varuint(
            bytes,
            static_cast<std::uint64_t>(right - left)
        );
        source_zip_internal::append_varuint<U>(
            bytes,
            source_zip_internal::encode_integer(values[left])
        );
        left = right;
    }
    return source_zip_internal::encode_base64(bytes);
}

template<class T>
std::vector<T> source_unzip(std::string_view encoded){
    static_assert(std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>);
    using U = std::make_unsigned_t<T>;
    const std::vector<unsigned char> bytes = source_zip_internal::decode_base64(encoded);
    std::size_t position = 0;
    const std::uint64_t count = source_zip_internal::read_varuint<std::uint64_t>(bytes, position);
    if(count > std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::runtime_error("library assertion fault: source zip size overflow.");
    }
    std::vector<T> result;
    result.reserve(static_cast<std::size_t>(count));
    while(result.size() < count){
        const std::uint64_t run = source_zip_internal::read_varuint<std::uint64_t>(bytes, position);
        if(run == 0 || run > count - result.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid source zip run length.");
        }
        const U encoded_value = source_zip_internal::read_varuint<U>(bytes, position);
        const T value = source_zip_internal::decode_integer<T>(encoded_value);
        result.insert(result.end(), static_cast<std::size_t>(run), value);
    }
    if(position != bytes.size())[[unlikely]]{
        throw std::runtime_error("library assertion fault: trailing source zip data.");
    }
    return result;
}

#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_PARTIALLY_PERSISTENT_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_PARTIALLY_PERSISTENT_ARRAY_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "../other/partially_persistent_storage.hpp"

template<class T, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentArray{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

private:
    int _n = 0;
    int version_count = 1;
    PartiallyPersistentStorage<T, MAX_SIZE, MAX_UPDATE> data;

    void check_size(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = n;
    }

public:
    PartiallyPersistentArray(const PartiallyPersistentArray&) = delete;
    PartiallyPersistentArray& operator=(const PartiallyPersistentArray&) = delete;
    PartiallyPersistentArray(PartiallyPersistentArray&&) = delete;
    PartiallyPersistentArray& operator=(PartiallyPersistentArray&&) = delete;

    explicit PartiallyPersistentArray(int n = MAX_SIZE){
        check_size(n);
        for(int k = 0; k < _n; k++) data.initialize(k, T{});
    }
    PartiallyPersistentArray(int n, const T& value){
        check_size(n);
        for(int k = 0; k < _n; k++) data.initialize(k, value);
    }
    explicit PartiallyPersistentArray(const std::vector<T>& values){
        check_size(static_cast<int>(values.size()));
        for(int k = 0; k < _n; k++){
            data.initialize(k, values[static_cast<std::size_t>(k)]);
        }
    }
    template<std::size_t N>
    explicit PartiallyPersistentArray(const std::array<T, N>& values){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        check_size(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            data.initialize(k, values[static_cast<std::size_t>(k)]);
        }
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }

    int set(int k, const T& value){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        if(version_count > MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (set).");
        }
        data.write(k, version_count, value);
        return version_count++;
    }
    const T& get(int version, int k) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return data.get(k, version);
    }
    const T& get(int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return data.current(k);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ARRAY_PARTIALLY_PERSISTENT_ARRAY_HPP_INCLUDED

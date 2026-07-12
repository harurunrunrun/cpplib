#pragma once

#include <array>
#include <cstddef>
#include <vector>
#include "persistent_array.hpp"

template<class T, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentArray{
private:
    PersistentArray<T, MAX_SIZE, MAX_UPDATE> data;

public:
    explicit PartiallyPersistentArray(int n = MAX_SIZE): data(n){}

    PartiallyPersistentArray(int n, const T& value): data(n, value){}

    explicit PartiallyPersistentArray(const std::vector<T>& values): data(values){}

    template<std::size_t N>
    explicit PartiallyPersistentArray(const std::array<T, N>& values): data(values){}

    int size() const{
        return data.size();
    }

    int versions() const{
        return data.versions();
    }

    int latest_version() const{
        return data.latest_version();
    }

    int set(int k, const T& value){
        return data.set(data.latest_version(), k, value);
    }

    const T& get(int version, int k) const{
        return data.get(version, k);
    }

    const T& get(int k) const{
        return data.get(data.latest_version(), k);
    }
};

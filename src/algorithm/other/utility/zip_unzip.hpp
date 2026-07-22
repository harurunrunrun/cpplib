#ifndef CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ZIP_UNZIP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ZIP_UNZIP_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
std::vector<std::pair<T, int>> run_length_zip(const std::vector<T>& data){
    std::vector<std::pair<T, int>> result;
    for(const T& value: data){
        if(result.empty() || !(result.back().first == value)){
            result.push_back({value, 1});
        }else{
            result.back().second++;
        }
    }
    return result;
}

template<class T>
std::vector<T> run_length_unzip(const std::vector<std::pair<T, int>>& data){
    std::vector<T> result;
    int total = 0;
    for(const auto& [value, count]: data){
        if(count < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: negative length (run_length_unzip).");
        }
        total += count;
    }
    result.reserve(static_cast<std::size_t>(total));
    for(const auto& [value, count]: data){
        for(int i = 0; i < count; i++){
            result.push_back(value);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ZIP_UNZIP_HPP_INCLUDED

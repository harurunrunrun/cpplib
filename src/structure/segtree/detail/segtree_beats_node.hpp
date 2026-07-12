#pragma once

#include <algorithm>

namespace segtree_beats_detail{

template<class T>
struct Node{
    T sum{};
    T max_value{};
    T second_max_value{};
    T min_value{};
    T second_min_value{};
    T add{};
    int max_count = 0;
    int min_count = 0;
    int length = 0;
};

template<class T>
Node<T> leaf(const T& value){
    Node<T> result;
    result.sum = value;
    result.max_value = value;
    result.min_value = value;
    result.max_count = 1;
    result.min_count = 1;
    result.length = 1;
    return result;
}

template<class T>
Node<T> merge(const Node<T>& left, const Node<T>& right){
    if(left.length == 0){
        Node<T> result = right;
        result.add = T{};
        return result;
    }
    if(right.length == 0){
        Node<T> result = left;
        result.add = T{};
        return result;
    }
    Node<T> result;
    result.sum = left.sum + right.sum;
    result.length = left.length + right.length;
    if(left.max_value == right.max_value){
        result.max_value = left.max_value;
        result.max_count = left.max_count + right.max_count;
        bool ls = left.max_count < left.length;
        bool rs = right.max_count < right.length;
        if(ls && rs) result.second_max_value = std::max(left.second_max_value, right.second_max_value);
        else if(ls) result.second_max_value = left.second_max_value;
        else if(rs) result.second_max_value = right.second_max_value;
    }else if(left.max_value > right.max_value){
        result.max_value = left.max_value;
        result.max_count = left.max_count;
        result.second_max_value = right.max_value;
        if(left.max_count < left.length){
            result.second_max_value = std::max(result.second_max_value, left.second_max_value);
        }
    }else{
        result.max_value = right.max_value;
        result.max_count = right.max_count;
        result.second_max_value = left.max_value;
        if(right.max_count < right.length){
            result.second_max_value = std::max(result.second_max_value, right.second_max_value);
        }
    }
    if(left.min_value == right.min_value){
        result.min_value = left.min_value;
        result.min_count = left.min_count + right.min_count;
        bool ls = left.min_count < left.length;
        bool rs = right.min_count < right.length;
        if(ls && rs) result.second_min_value = std::min(left.second_min_value, right.second_min_value);
        else if(ls) result.second_min_value = left.second_min_value;
        else if(rs) result.second_min_value = right.second_min_value;
    }else if(left.min_value < right.min_value){
        result.min_value = left.min_value;
        result.min_count = left.min_count;
        result.second_min_value = right.min_value;
        if(left.min_count < left.length){
            result.second_min_value = std::min(result.second_min_value, left.second_min_value);
        }
    }else{
        result.min_value = right.min_value;
        result.min_count = right.min_count;
        result.second_min_value = left.min_value;
        if(right.min_count < right.length){
            result.second_min_value = std::min(result.second_min_value, right.second_min_value);
        }
    }
    return result;
}

template<class T>
void add(Node<T>& node, const T& value){
    if(node.length == 0) return;
    node.sum += value * static_cast<T>(node.length);
    node.max_value += value;
    if(node.max_count < node.length) node.second_max_value += value;
    node.min_value += value;
    if(node.min_count < node.length) node.second_min_value += value;
    node.add += value;
}

template<class T>
void chmin(Node<T>& node, const T& value){
    if(node.length == 0 || node.max_value <= value) return;
    T old = node.max_value;
    node.sum += (value - old) * static_cast<T>(node.max_count);
    if(node.min_value == old) node.min_value = value;
    else if(node.min_count < node.length && node.second_min_value == old){
        node.second_min_value = value;
    }
    node.max_value = value;
}

template<class T>
void chmax(Node<T>& node, const T& value){
    if(node.length == 0 || value <= node.min_value) return;
    T old = node.min_value;
    node.sum += (value - old) * static_cast<T>(node.min_count);
    if(node.max_value == old) node.max_value = value;
    else if(node.max_count < node.length && node.second_max_value == old){
        node.second_max_value = value;
    }
    node.min_value = value;
}

template<class T>
void inherit(const Node<T>& parent, Node<T>& child){
    add(child, parent.add);
    chmin(child, parent.max_value);
    chmax(child, parent.min_value);
}

} // namespace segtree_beats_detail

---
title: Sliding Window K Smallest Sums (各スライド窓のK最小和) [ABC281_E]
documentation_of: ../src/algorithm/sequence/sliding_window_k_smallest_sums.hpp
---

固定長の各連続部分列について、比較順で小さい方から `k` 個の射影値の和を返す。

## `sliding_window_k_smallest_sums`

```cpp
template<class T, class Sum = T, class Compare = less<T>, class Projection = ...>
vector<Sum> sliding_window_k_smallest_sums(
    const vector<T>& values,
    size_t window_size,
    size_t k,
    const Compare& compare = Compare(),
    const Projection& project = Projection()
)
```

返り値の `i` 番目は `values[i, i + window_size)` のうち、`compare` 順で先頭の
`k` 要素を `project` して加算した値である。返り値の長さは
`values.size() - window_size + 1` となる。同値な要素も個別に数える。

## 時間計算量

`N = values.size()`、`W = window_size` とすると
$O(N\log(W+1))$。

## 空間計算量

作業領域は $O(W)$、返り値を含めて $O(N)$。

## 注意点

- `1 <= window_size <= values.size()` かつ `k <= window_size` が必要であり、違反時は `runtime_error`。
- `Sum{}` が加法単位元であり、`Sum` は `operator+=` を提供する必要がある。
- `Compare` は strict weak ordering、`Projection` は `T` から `Sum` への写像でなければならない。
- 加算のoverflowは `Sum` の演算規則に従う。

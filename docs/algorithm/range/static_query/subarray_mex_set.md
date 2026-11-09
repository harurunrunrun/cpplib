---
title: Subarray MEX Value Set (部分配列MEX値集合)
documentation_of: ../../../../src/algorithm/range/static_query/subarray_mex_set.hpp
---

すべての非空連続部分列の MEX として現れる値の集合を求める。
値ごとの連続する出現位置の間隔と、各値の最終出現位置の区間最小値（セグメント木）を用いる。

## subarray_mex_values

```cpp
vector<size_t> subarray_mex_values(const vector<int64_t>& values, size_t first = 0)
```

`first = 0` では通常の MEX（含まれない最小の非負整数）、`first = 1` では含まれない最小の正整数を MEX とみなし、
得られる値の集合を昇順に返す。

## mex_of_subarray_mex_values

```cpp
size_t mex_of_subarray_mex_values(const vector<int64_t>& values, size_t first = 0)
```

上記の値集合に含まれない `first` 以上の最小の整数を返す。

## 時間計算量

$N$ を `values.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `subarray_mex_values` | $O(N\log N)$ | $O(N)$ |
| `mex_of_subarray_mex_values` | $O(N\log N)$ | $O(N)$ |

## 注意点

`first` は 0 または 1 でなければならず、違反時は `std::invalid_argument` を送出する。空の部分列は対象にしない。

---
title: Subarray MEX Value Set (部分配列MEX値集合)
documentation_of: ../../../../src/algorithm/range/static_query/subarray_mex_set.hpp
---

全ての非空連続部分列から得られる MEX 値の集合を、値ごとの連続出現間隔と最終出現位置の区間最小で求める。

## subarray_mex_values

```cpp
vector<size_t> subarray_mex_values(const vector<int64_t>& values, size_t first = 0)
```

`first=0` では通常の非負 MEX、`first=1` では正整数 MEX の値集合を昇順に返す。

## mex_of_subarray_mex_values

```cpp
size_t mex_of_subarray_mex_values(const vector<int64_t>& values, size_t first = 0)
```

上記の値集合に含まれない最小の `first` 以上の整数を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `subarray_mex_values` | $O(N\log N)$ | $O(N)$ |
| `mex_of_subarray_mex_values` | $O(N\log N)$ | $O(N)$ |

## 注意点

`first` は 0 または 1 でなければならず、違反時は `invalid_argument` を送出する。空部分列は対象にしない。

---
title: Minimum Excluded Value (最小除外値・MEX)
documentation_of: ../../../../src/algorithm/range/static_query/minimum_excluded.hpp
---

整数列に含まれない最小の非負整数を求める。

## minimum_excluded

```cpp
template<integral Integer>
size_t minimum_excluded(const vector<Integer>& values)
```

負数と `values.size()` より大きい値は答えに影響しないため無視する。重複を許す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_excluded` | $O(N)$ | $O(N)$ |

## 注意点

`Integer` は整数型でなければならない。戻り値は常に `[0,N]` に入る。

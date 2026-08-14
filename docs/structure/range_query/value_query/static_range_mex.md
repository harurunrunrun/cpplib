---
title: Static Range MEX (静的区間MEX)
documentation_of: ../../../../src/structure/range_query/value_query/static_range_mex.hpp
---

静的整数列の任意の半開区間 MEX を、右端ごとの永続最終出現 segment tree で求める。

## constructor

```cpp
explicit StaticRangeMex(const vector<int64_t>& values)
```

各 prefix の version を構築する。

## range_mex

```cpp
size_t range_mex(size_t left, size_t right) const
```

`values[left:right]` の MEX を返す。空区間は 0 を返す。

## size / node_count

列長と確保 node 数を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(N\log N)$ | $O(N\log N)$ |
| `range_mex` | $O(\log N)$ | $O(\log N)$ 再帰 stack |
| `size`, `node_count` | $O(1)$ | $O(1)$ |

## 注意点

MEX は `[0,N]` に入るため、負数と `N` より大きい値は更新しない。不正区間は `out_of_range` を送出する。

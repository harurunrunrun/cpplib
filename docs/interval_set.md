---
title: Interval Set
documentation_of: ../src/algorithm/range/interval_set.hpp
---

半開区間 $[l, r)$ の集合。

区間は常に互いに素で、隣接する区間は併合される。

# コンストラクタ

```cpp
IntervalSet<T> intervals
```

# 関数

```cpp
intervals.add(l, r)
intervals.erase(l, r)
intervals.contains(x)
intervals.covered(l, r)
intervals.mex(x)
intervals.intervals()
```

`mex(x)` は `x` 以上で集合に含まれない最小の値を返す。

## 時間計算量

- $O(K \log N)$

`K` は操作で消える区間数。

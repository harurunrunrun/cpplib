---
title: One-Dimensional Bin Packing Heuristics
documentation_of: ../src/approximate/packing/bin_packing.hpp
---

容量が等しいbinへitemを詰めるonline/decreasing heuristicを
`approximate::packing`名前空間で提供する。

## `BinPackingResult`

```cpp
template<class Size>
struct BinPackingResult {
    vector<size_t> bin_of_item;
    vector<Size> remaining_capacity;
    size_t bin_count() const;
};
```

各itemのbin番号と各binの残容量を持つ。`bin_count`とmember参照は$O(1)$、
全結果の列挙は$O(N+B)$。

## packing API

```cpp
next_fit_bin_packing(items, capacity, decreasing = false)
first_fit_bin_packing(items, capacity, decreasing = false)
best_fit_bin_packing(items, capacity, decreasing = false)
worst_fit_bin_packing(items, capacity, decreasing = false)
```

- Next Fit: 現在binに入らなければ新しいbinを開く。
- First Fit: 入るbinのうち番号最小を選ぶ。
- Best Fit: 詰めた後の残容量が最小のbinを選ぶ。
- Worst Fit: 詰める前の残容量が最大のbinを選ぶ。

`decreasing=true`ではitemを大きさの降順（同値は元添字順）へ並べてから実行する。
返す`bin_of_item`は常に元のitem添字に対応する。First Fitはmax segment tree、
Best/Worst Fitは平衡二分木でbinを選ぶ。

## 計算量

| API | `decreasing=false` | `decreasing=true` | 追加領域 |
| --- | --- | --- | --- |
| Next Fit | $O(N)$ | $O(N\log N)$ | $O(N)$ |
| First Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |
| Best Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |
| Worst Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |

容量が非正、itemが負、itemが容量を超える場合は`std::invalid_argument`。
`Size`は有限な全順序付き加減算型で、減算結果を表現できなければならない。

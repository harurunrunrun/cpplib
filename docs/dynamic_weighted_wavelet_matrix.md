---
title: Dynamic Weighted Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/dynamic_weighted_wavelet_matrix.hpp
---

点更新できる重み付き整数列。
値に対する順序統計と、値の範囲で絞った重みの和を扱う。

# set

```cpp
wm.set(k, value, weight)
wm.set_value(k, value)
wm.set_weight(k, weight)
```

# 取得

```cpp
wm.access(k)
wm[k]
wm.weight(k)
```

# 和

```cpp
wm.sum(l, r)
wm.range_sum(l, r, upper)
wm.range_sum(l, r, lower, upper)
wm.sum_k_smallest(l, r, k)
wm.sum_k_largest(l, r, k)
```

`sum_k_smallest` と `sum_k_largest` は値が等しい場合、元の添字順で選ぶ。

# 時間計算量

$B = BLOCK\_SIZE$ とする。

- `set`: $O(B\log B)$
- `rank`, `range_freq`, `range_sum`: $O(B + \frac{N}{B}\log B)$
- `kth_smallest`, `kth_largest`: $O(BIT\_WIDTH \cdot (B + \frac{N}{B}\log B))$
- `sum_k_smallest`, `sum_k_largest`: `kth_smallest` に加えて、同値の探索に $O(r-l)$

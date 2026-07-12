---
title: Persistent Weighted Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp
---

完全永続の重み付きwavelet matrix。任意のバージョンから値と重みを更新できる。

# 更新

```cpp
wm.set(version, k, value, weight)
wm.set_value(version, k, value)
wm.set_weight(version, k, weight)
wm.fork(version)
```

# 和

```cpp
wm.sum(version, l, r)
wm.range_sum(version, l, r, upper)
wm.range_sum(version, l, r, lower, upper)
wm.sum_k_smallest(version, l, r, k)
wm.sum_k_largest(version, l, r, k)
```

# 時間計算量

`PersistentWaveletMatrix` と同じブロック分割。`sum_k_smallest` は同値の処理に $O(r-l)$ かかる。

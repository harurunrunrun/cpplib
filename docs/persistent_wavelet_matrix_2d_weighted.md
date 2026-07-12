---
title: Persistent Weighted Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d_weighted.hpp
---

x座標を固定し、y座標と重みを完全永続に更新できる2次元wavelet matrix。

```cpp
wm.set(version, k, y, weight)
wm.set_y(version, k, y)
wm.set_weight(version, k, weight)
wm.rectangle_count(version, xl, xr, yl, yr)
wm.rectangle_sum(version, xl, xr, yl, yr)
```
# 時間計算量

x 座標の範囲取得に $O(\log N)$、以降は
`PersistentWeightedWaveletMatrix` と同じ。
`set`, `set_y`, `set_weight` は $O(B\log B + \log M)$、
`fork` は $O(1)$。

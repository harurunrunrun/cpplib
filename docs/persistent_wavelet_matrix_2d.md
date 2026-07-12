---
title: Persistent Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を完全永続に更新できる2次元wavelet matrix。

# 更新

```cpp
wm.set_y(version, k, y)
wm.fork(version)
```

元の点番号 `k` のy座標を更新する。x座標は変えない。

# クエリ

```cpp
wm.rectangle_count(version, xl, xr, yl, yr)
wm.range_freq(version, xl, xr, yl, yr)
wm.kth_smallest_y(version, xl, xr, k)
wm.prev_y(version, xl, xr, upper)
wm.next_y(version, xl, xr, lower)
```
# 時間計算量

x 座標の範囲取得に $O(\log N)$、以降は `PersistentWaveletMatrix` と同じ。
`set_y` は $O(B\log B + \log M)$、`fork` は $O(1)$。

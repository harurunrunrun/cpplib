---
title: Guillotine Rectangle Packing (guillotine長方形packing)
documentation_of: ../src/approximate/packing/guillotine_packing.hpp
---

## Guillotine Rectangle Packing (guillotine長方形packing)

自由長方形を水平または垂直に二分しながらbest-area-fitで配置するheuristic。

## API

### `GuillotineSplit`

分割方向を `shorter_leftover_axis`, `longer_leftover_axis`, `shorter_axis`, `longer_axis` から選ぶ列挙型。

### `guillotine_best_area_fit_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation=false, split=GuillotineSplit::shorter_leftover_axis)`

面積の無駄、短辺余り、bin番号の順で候補を選ぶ。長方形数を $N$、全binの自由長方形数の最大値を $F=O(N)$ として時間計算量は $O(NF)=O(N^2)$、返値を含む空間計算量は $O(N+F)=O(N)$。

## 注意点

寸法は正の有限値で、各長方形は許可された向きで空binに収まらなければならない。不正入力では `std::invalid_argument` を送出する。guillotine分割可能な配置だけを探索し、最適性は保証しない。

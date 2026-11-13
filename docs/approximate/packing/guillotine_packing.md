---
title: Guillotine Rectangle Packing (ギロチン分割長方形パッキング)
documentation_of: ../../../src/approximate/packing/guillotine_packing.hpp
---

空き長方形を水平または垂直に二分しながら、best-area-fit で長方形を配置するヒューリスティックである。

## API

### `GuillotineSplit`

分割方向の決め方を `shorter_leftover_axis`, `longer_leftover_axis`, `shorter_axis`, `longer_axis` から選ぶ列挙型。

### `guillotine_best_area_fit_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation = false, split = GuillotineSplit::shorter_leftover_axis)`

無駄になる面積、短辺側の余り、ビン番号の順に比較して候補を選ぶ。長方形数を $N$、全ビンの空き長方形数の最大値を $F = O(N)$ として、時間計算量は $O(NF) = O(N^2)$、返値を含む空間計算量は $O(N + F) = O(N)$。

## 注意点

- 寸法は正の有限値で、各長方形は許された向きで空のビンに収まらなければならない。不正な入力では `std::invalid_argument` を送出する。
- ギロチン分割で得られる配置だけを探索し、最適性は保証しない。

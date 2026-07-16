---
title: Skyline Rectangle Packing (skyline長方形packing)
documentation_of: ../src/approximate/packing/skyline_packing.hpp
---

## Skyline Rectangle Packing (skyline長方形packing)

各binの上端を区分一定なskylineとして管理し、候補位置へ長方形を配置するheuristic。

## API

### `SkylineFit`

評価規則を `bottom_left`, `best_fit` から選ぶ列挙型。

### `skyline_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation=false)`

全skyline nodeを調べ、bottom-leftまたは埋まらない面積で候補を選ぶ。長方形数を $N$、全binのskyline node数の最大値を $S=O(N)$ として時間計算量は $O(NS^2)=O(N^3)$、返値を含む空間計算量は $O(N+S)=O(N)$。

### `skyline_bottom_left_rectangle_packing(...)`, `skyline_best_fit_rectangle_packing(...)`

各 `SkylineFit` を選ぶshortcut。引数と計算量は `skyline_rectangle_packing` と同じ。

## 注意点

寸法は正の有限値で、各長方形は許可された向きで空binに収まらなければならない。不正入力では `std::invalid_argument` を送出する。最適bin数の保証はない。

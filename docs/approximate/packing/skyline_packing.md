---
title: Skyline Rectangle Packing (スカイライン長方形パッキング)
documentation_of: ../../../src/approximate/packing/skyline_packing.hpp
---

各ビンの上端を区分的に一定なスカイラインとして管理し、候補位置へ長方形を配置するヒューリスティックである。

## API

### `SkylineFit`

評価規則を `bottom_left`, `best_fit` から選ぶ列挙型。

### `skyline_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation = false)`

すべてのスカイラインの節を調べ、bottom-left（最も低く左の位置）または埋められなくなる面積の小ささで候補を選ぶ。長方形数を $N$、全ビンのスカイラインの節数の最大値を $S = O(N)$ として、時間計算量は $O(NS^2) = O(N^3)$、返値を含む空間計算量は $O(N + S) = O(N)$。

### `skyline_bottom_left_rectangle_packing(...)`, `skyline_best_fit_rectangle_packing(...)`

対応する `SkylineFit` を指定する短縮形。引数は `fit` を除いて `skyline_rectangle_packing` と同じで、計算量も同じ。

## 注意点

- 寸法は正の有限値で、各長方形は許された向きで空のビンに収まらなければならない。不正な入力では `std::invalid_argument` を送出する。
- 使用ビン数の最適性は保証しない。

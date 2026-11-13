---
title: MaxRects Rectangle Packing (MaxRects 長方形パッキング)
documentation_of: ../../../src/approximate/packing/maxrects_packing.hpp
---

重なりを許す空き長方形の集合を、分割と包含除去で管理しながら配置する MaxRects ヒューリスティックである。

## API

### `MaxRectsFit`

候補の評価尺度を `best_short_side_fit`, `best_long_side_fit`, `best_area_fit` から選ぶ列挙型。

### `maxrects_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation = false)`

指定した尺度で最良の空き長方形を選ぶ。長方形数を $N$、全ビンの空き長方形数の最大値を $F$ として、時間計算量は $O(NF^2)$、返値を含む空間計算量は $O(N + F)$。$F$ は入力に応じて増えるため、$N$ だけで表した最悪計算量は大きくなり得る。

### `maxrects_best_short_side_fit(rectangles, bin_width, bin_height, allow_rotation = false)`, `maxrects_best_long_side_fit(...)`, `maxrects_best_area_fit(...)`

対応する `MaxRectsFit` を指定した短縮形。引数は `fit` を除いて `maxrects_rectangle_packing` と同じで、計算量も同じ。

## 注意点

- 寸法は正の有限値で、各長方形は許された向きで空のビンに収まらなければならない。不正な入力では `std::invalid_argument` を送出する。
- 空き長方形の数は入力によって大きく増えることがある。
- 使用ビン数の最適性は保証しない。

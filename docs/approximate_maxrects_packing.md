---
title: MaxRects Rectangle Packing (MaxRects長方形packing)
documentation_of: ../src/approximate/packing/maxrects_packing.hpp
---

## MaxRects Rectangle Packing (MaxRects長方形packing)

重なりを許す自由長方形集合を分割・包含除去しながら配置するMaxRects heuristic。

## API

### `MaxRectsFit`

候補評価を `best_short_side_fit`, `best_long_side_fit`, `best_area_fit` から選ぶ列挙型。

### `maxrects_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation=false)`

指定尺度で最良の自由長方形を選ぶ。長方形数を $N$、全binの自由長方形数の最大値を $F$ として時間計算量は $O(NF^2)$、空間計算量は返値を含め $O(N+F)$。この実装では $F$ が入力に対して増えるため最悪計算量は多項式より大きくなり得る。

### `maxrects_best_short_side_fit(...)`, `maxrects_best_long_side_fit(...)`, `maxrects_best_area_fit(...)`

各 `MaxRectsFit` を選ぶshortcut。引数と計算量は `maxrects_rectangle_packing` と同じ。

## 注意点

寸法は正の有限値で、各長方形は許可された向きで空binに収まらなければならない。不正入力では `std::invalid_argument` を送出する。自由領域数は入力によって大きく増える。最適bin数の保証はない。

---
title: Bottom-Left Rectangle Packing (左下詰め長方形packing)
documentation_of: ../src/approximate/packing/bottom_left_packing.hpp
---

# Bottom-Left Rectangle Packing (左下詰め長方形packing)

入力順に各長方形を既存bin内の最も下、同じ高さなら最も左の候補へ配置するheuristic。

## API

### `bottom_left_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation=false, fill_holes=false)`

`fill_holes=false` では配置済み長方形の辺から作る候補点を調べ、`true` では全x端点とy端点の直積を調べる。長方形数を $N$、ある時点の配置済み数を $i$ とすると、通常版の最悪時間計算量は $O(N^4)$、穴埋め版は $O(N^5)$、返値を含む空間計算量は $O(N)$、候補点の一時空間はそれぞれ $O(N)$、$O(N^2)$。

### `bottom_left_fill_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation=false)`

`fill_holes=true` のshortcut。計算量は穴埋め版 `bottom_left_rectangle_packing` と同じ。

## 注意点

`Size` は比較、四則演算、`long double` への変換が可能でなければならない。binと長方形の寸法は正の有限値で、各長方形は回転を許す場合も含め空binに収まらなければならない。不正入力では `std::invalid_argument` を送出する。最適bin数の保証はない。

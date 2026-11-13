---
title: Bottom-Left Rectangle Packing (左下詰め長方形パッキング)
documentation_of: ../../../src/approximate/packing/bottom_left_packing.hpp
---

入力順に、各長方形を既存のビンの中で最も下、同じ高さなら最も左の候補位置へ配置するヒューリスティックである。

## API

### `bottom_left_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation = false, fill_holes = false)`

`fill_holes = false` では配置済み長方形の辺から作る候補点を調べ、`true` ではすべての $x$ 端点と $y$ 端点の直積を候補点として調べる（穴埋め版）。

長方形数を $N$ とすると、最悪時間計算量は通常版が $O(N^3)$、穴埋め版が $O(N^4)$ である（配置済み $k$ 個のビン 1 つにつき候補点 $O(k)$ 個または $O(k^2)$ 個のそれぞれで $O(k)$ の重なり判定を行う）。返値を含む空間計算量は $O(N)$ で、候補点の一時領域は通常版が $O(N)$、穴埋め版が $O(N^2)$ である。

### `bottom_left_fill_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation = false)`

`fill_holes = true` を指定した `bottom_left_rectangle_packing` の短縮形。計算量は穴埋め版と同じ。

## 注意点

- `Size` は比較、四則演算、`long double` への変換ができなければならない。
- ビンと長方形の寸法は正の有限値で、各長方形は（回転を許す場合はいずれかの向きで）空のビンに収まらなければならない。不正な入力では `std::invalid_argument` を送出する。
- 使用ビン数の最適性は保証しない。

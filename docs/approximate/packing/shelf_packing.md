---
title: Shelf Rectangle Packing (棚型長方形パッキング)
documentation_of: ../../../src/approximate/packing/shelf_packing.hpp
---

ビンを水平な棚（shelf）に分け、各長方形をいずれかの棚に配置するヒューリスティックである。

## API

### `ShelfFit`

棚の選び方を `next_fit`, `first_fit`, `best_width_fit`, `best_height_fit` から選ぶ列挙型。

### `shelf_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation = false, decreasing_height = false)`

指定した規則で棚を選ぶ。`decreasing_height = true` のときは、高さの降順、同じなら幅の降順に安定ソートしてから配置する。長方形数を $N$ として、時間計算量は `next_fit` が $O(N \log N)$（ソートしない場合は $O(N)$）、それ以外が最悪 $O(N^2 + N \log N)$ である。返値を含む空間計算量は $O(N)$。

### `shelf_next_fit(...)`, `shelf_first_fit(...)`, `shelf_best_width_fit(...)`, `shelf_best_height_fit(...)`

入力順のまま、対応する `ShelfFit` を指定する短縮形。引数は `(rectangles, bin_width, bin_height, allow_rotation = false)` で、計算量は対応する `shelf_rectangle_packing` と同じ。

### `nfdh_rectangle_packing(...)`, `ffdh_rectangle_packing(...)`, `bfdh_rectangle_packing(...)`

高さの降順に並べたうえで、それぞれ next-fit、first-fit、best-width-fit を使う短縮形。引数は上と同じ。時間計算量は NFDH が $O(N \log N)$、FFDH と BFDH が $O(N^2)$、空間計算量は $O(N)$。

## 注意点

- 寸法は正の有限値で、各長方形は許された向きで空のビンに収まらなければならない。不正な入力では `std::invalid_argument` を送出する。
- 回転を許す場合も、高さ降順の並びは入力時の向きの寸法で決まる。
- 最適性は保証しない。

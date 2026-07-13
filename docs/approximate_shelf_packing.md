---
title: Shelf Rectangle Packing (棚型長方形packing)
documentation_of: ../src/approximate/packing/shelf_packing.hpp
---

# Shelf Rectangle Packing (棚型長方形packing)

binを水平なshelfへ分け、各長方形をいずれかのshelfへ配置するheuristic。

## API

### `ShelfFit`

探索規則を `next_fit`, `first_fit`, `best_width_fit`, `best_height_fit` から選ぶ列挙型。

### `shelf_rectangle_packing(rectangles, bin_width, bin_height, fit, allow_rotation=false, decreasing_height=false)`

指定規則でshelfを選ぶ。`decreasing_height=true` なら高さ、幅の降順で安定sortしてから配置する。長方形数を $N$ として `next_fit` の時間計算量は $O(N\log N)$（sortなしなら $O(N)$）、ほかは最悪 $O(N^2+N\log N)$、返値を含む空間計算量は $O(N)$。

### `shelf_next_fit(...)`, `shelf_first_fit(...)`, `shelf_best_width_fit(...)`, `shelf_best_height_fit(...)`

入力順で各 `ShelfFit` を選ぶshortcut。計算量は対応する `shelf_rectangle_packing` と同じ。

### `nfdh_rectangle_packing(...)`, `ffdh_rectangle_packing(...)`, `bfdh_rectangle_packing(...)`

高さ降順のnext-fit、first-fit、best-width-fitを選ぶshortcut。時間計算量はNFDHが $O(N\log N)$、FFDH/BFDHが $O(N^2)$、空間計算量は $O(N)$。

## 注意点

寸法は正の有限値で、各長方形は許可された向きで空binに収まらなければならない。不正入力では `std::invalid_argument` を送出する。回転時のdecreasing-height順序は入力時の向きで決まる。最適性は保証しない。

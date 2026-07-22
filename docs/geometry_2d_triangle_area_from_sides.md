---
title: Triangle Area from Three Sides (三辺から求める三角形面積)
documentation_of: ../src/algorithm/geometry/2d/scalar/triangle_area_from_sides.hpp
---

## API

`long double triangle_area_from_sides(first, second, third)`

## 引数

有限な非負の3辺長を渡す。

## 戻り値

Heronの公式を数値的に並べ替えて求めた非負面積。

## API別の時間計算量・空間計算量

- `triangle_area_from_sides`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

三角不等式を満たさない場合と非有限値・負辺長では `invalid_argument`。退化三角形は `0`。

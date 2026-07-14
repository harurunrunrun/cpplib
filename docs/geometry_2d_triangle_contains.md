---
title: Inclusive Point in Triangle (三角形の内部・境界判定)
documentation_of: ../src/algorithm/geometry/2d/triangle_contains.hpp
---

## API

`bool triangle_contains(first, second, third, point)`

## 引数

三角形の3頂点と判定点を渡す。

## 戻り値

点が内部または境界上にあるとき `true`。

## API別の時間計算量・空間計算量

- `triangle_contains`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化三角形では3辺の和集合上にあるかを判定する。
辺上判定と点対辺の符号には、辺長で正規化した有向距離に対する `GEOMETRY_EPS` を用いる。三角形自体の退化は辺同士の角度で判定する。

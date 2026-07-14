---
title: Strict Point in Triangle (三角形の内部判定)
documentation_of: ../src/algorithm/geometry/2d/triangle_contains_strict.hpp
---

## API

`bool triangle_contains_strict(first, second, third, point)`

## 引数

三角形の3頂点と判定点を渡す。

## 戻り値

点が境界を除く内部にあるとき `true`。

## API別の時間計算量・空間計算量

- `triangle_contains_strict`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

頂点順は時計回りでもよい。退化三角形では常に `false`。
境界除外と点対辺の符号には、辺長で正規化した有向距離に対する `GEOMETRY_EPS` を用いる。三角形自体の退化は辺同士の角度で判定する。

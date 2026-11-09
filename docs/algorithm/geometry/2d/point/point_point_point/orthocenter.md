---
title: 2D Triangle Orthocenter (二次元三角形の垂心)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point_point_point/orthocenter.hpp
---

## API

`Point orthocenter(first, second, third)`

## 引数

三角形の 3 頂点を渡す。

## 戻り値

3 本の垂線が交わる点（垂心）を返す。外心を $O$ として $H = A + (B - O) + (C - O)$ で求める。

## API別の時間計算量・空間計算量

- `orthocenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3 点が共線なら、内部で呼ぶ `circumcenter` が `std::invalid_argument` を送出する。
外心からの差分を最初の頂点に加える形で計算するので、3 頂点の絶対座標の和を中間計算することはない。

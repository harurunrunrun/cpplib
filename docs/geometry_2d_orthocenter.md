---
title: 2D Triangle Orthocenter (二次元三角形の垂心)
documentation_of: ../src/algorithm/geometry/2d/point/orthocenter.hpp
---

## API

`Point orthocenter(first, second, third)`

## 引数

三角形の3頂点を渡す。

## 戻り値

3本の垂線が交わる点を返す。

## API別の時間計算量・空間計算量

- `orthocenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3点が共線なら `std::invalid_argument`。外心からの差分を最初の頂点へ加える形で計算し、3頂点の絶対座標和を中間計算しない。

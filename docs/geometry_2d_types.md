---
title: 2D Geometry Types (二次元幾何の基本型)
documentation_of: ../src/algorithm/geometry/2d/types.hpp
---

点、直線、線分、円を表す基本型。

## API

- `Point{x, y}`: `long double` 座標を持つ。既定構築、四則演算、複合代入、辞書順比較、許容誤差付き等値比較を提供する。
- `operator*(scalar, point)`: 左からのスカラー倍。
- `Line{a, b}`: 2点を通る直線。
- `Segment`: `Line` の別名で、端点 `a`, `b` を持つ線分。
- `Circle{center, radius}`: 中心と半径を持つ円。

## API別の時間計算量・空間計算量

- 各構築、演算、比較、field参照: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`Point::operator==` は許容誤差付きだが、`operator<` は座標の厳密な辞書順である。除算の除数、直線の退化、円の半径は各利用APIの条件を満たす必要がある。

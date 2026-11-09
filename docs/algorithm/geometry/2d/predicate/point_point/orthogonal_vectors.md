---
title: Vector Orthogonality (ベクトルの直交判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_point/orthogonal_vectors.hpp
---

2 本の二次元ベクトルが直交するか判定する。

## API

- `bool orthogonal(const Point& left, const Point& right)`: ベクトル `left` と `right` が直交すれば `true` を返す。

## API別の時間計算量・空間計算量

- `orthogonal`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 長さの積をスケールとする許容誤差付きで内積が $0$ かを判定する。
- 長さが `GEOMETRY_EPS` 以下の零ベクトルを含む場合は、互換性のため `true` を返す。

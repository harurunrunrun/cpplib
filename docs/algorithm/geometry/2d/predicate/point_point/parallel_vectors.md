---
title: Vector Parallelism (ベクトルの平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_point/parallel_vectors.hpp
---

2 本の二次元ベクトルが平行か判定する。

## API

- `bool parallel(const Point& left, const Point& right)`: ベクトル `left` と `right` が平行なら `true` を返す。逆向きの場合も平行とみなす。

## API別の時間計算量・空間計算量

- `parallel`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 長さの積をスケールとする許容誤差付きで外積が $0$ かを判定する。
- 長さが `GEOMETRY_EPS` 以下の零ベクトルを含む場合は、互換性のため `true` を返す。

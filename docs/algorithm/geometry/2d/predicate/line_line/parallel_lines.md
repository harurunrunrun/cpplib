---
title: Line Parallelism (直線の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_line/parallel_lines.hpp
---

2 本の無限直線の方向が平行か判定する。

## API

- `bool parallel(const Line& left, const Line& right)`: 直線 `left` と `right` が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- いずれかの方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線なら `std::invalid_argument` を送出する。
- 外積を方向ベクトルの長さの積で正規化して判定するため、方向ベクトルを $0$ でない定数倍しても判定は変わらない。

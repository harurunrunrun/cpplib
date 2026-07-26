---
title: Line Orthogonality (直線直交判定)
documentation_of: ../../../../src/algorithm/geometry/2d/orthogonal_lines.hpp
---

2本の無限直線の方向が直交するか判定する。

## API

- `orthogonal(left, right)`: 直線 `left` と `right` が直交すれば `true` を返す。

## API別の時間計算量・空間計算量

- `orthogonal`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

いずれかが退化直線なら `std::invalid_argument` を送出する。直線の方向ベクトルの非零scaleに対して判定は不変である。

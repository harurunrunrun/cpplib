---
title: Line Parallelism (直線平行判定)
documentation_of: ../../../../src/algorithm/geometry/2d/parallel_lines.hpp
---

2本の無限直線の方向が平行か判定する。

## API

- `parallel(left, right)`: 直線 `left` と `right` が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

いずれかが退化直線なら `std::invalid_argument` を送出する。直線の方向ベクトルの非零scaleに対して判定は不変である。

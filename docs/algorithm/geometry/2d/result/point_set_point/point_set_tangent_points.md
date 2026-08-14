---
title: Point Set Global Tangent Points (点集合への大域接点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_point/point_set_tangent_points.hpp
---

凸性や点順を仮定しない点集合に対し、外点から引いた2本の大域的な支持接線の接点を返す。全入力点を同じ閉半平面に置く接線を対象とする。

## API

- `point_set_tangent_points(points, source)`: 左右の接点を返す。空集合なら `std::nullopt`。

## API別の時間計算量・空間計算量

- 点数を $N$、凸包頂点数を $H$ として、凸包構築の時間計算量 $O(N\log N)$、接点探索 $O(\log H)$、追加空間計算量 $O(N)$。

## 注意点

`source` は点集合の凸包の外部でなければならず、内部・境界なら `std::invalid_argument` を送出する。凹頂点に対する局所接線ではなく、点集合全体の支持線を返す。

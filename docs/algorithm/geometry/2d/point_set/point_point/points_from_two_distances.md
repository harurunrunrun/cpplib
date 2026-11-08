---
title: Points from Two Distances (二点からの距離による点復元)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_point/points_from_two_distances.hpp
---

## API

`std::vector<Point> points_from_two_distances(first, first_distance, second, second_distance)`

## 引数

2 つの基準点 `first`, `second` と、それぞれから求める点までの距離 `first_distance`, `second_distance`（有限かつ非負）を渡す。

## 戻り値

両方の距離条件を満たす相異なる点を、$(x, y)$ の辞書順に $0$ ～ $2$ 個返す。

## API別の時間計算量・空間計算量

- `points_from_two_distances`: 時間 $O(1)$、戻り値の領域 $O(1)$。

## 注意点

- 距離が負または非有限なら `std::invalid_argument` を送出する。
- 2 つの基準点が一致する場合、距離が異なれば空列、どちらの距離も $0$ ならその点 1 個を返し、正の距離が等しく解が無数にあるなら `std::domain_error` を送出する。

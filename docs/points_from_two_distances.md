---
title: Points from Two Distances (二点からの距離による点復元)
documentation_of: ../src/algorithm/geometry/2d/point_collection/points_from_two_distances.hpp
---

## API

`vector<Point> points_from_two_distances(first, first_distance, second, second_distance)`

## 引数

2基準点と、それぞれから求める点までの有限な非負距離を渡す。

## 戻り値

両距離条件を満たす異なる点を辞書順に0～2個返す。

## API別の時間計算量・空間計算量

- `points_from_two_distances`: 時間 $O(1)$、戻り値領域 $O(1)$。

## 注意点

距離が負または非有限なら `invalid_argument`。2円が一致して解が無数なら `domain_error`。

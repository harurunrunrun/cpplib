---
title: Matroid Intersection (マトロイド交差)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_intersection.hpp
---

同じground set上の2マトロイドに共通して独立な集合の最大要素数を、交換グラフのaugmenting pathで求める。

## `MatroidIntersectionResult`

`elements` が最大共通独立集合。要素番号は `[0, ground_set_size)`。

## `matroid_intersection(ground_set_size, first_independent, second_independent)`

各oracleは、昇順の要素番号vectorを受け取り独立性を返す。最大rankを $r$、ground set sizeを $n$、oracle 1回の時間を $T$ とすると時間計算量 $O(rn^2T)$、空間計算量 $O(n)$。

## 注意点

oracleはマトロイド公理を満たし、空集合を受理する必要がある。空集合を拒否した場合は `std::invalid_argument` を送出する。

---
title: Douglas–Peucker Algorithm (Douglas–Peucker法)
documentation_of: ../../../src/approximate/geometry/douglas_peucker.hpp
---

折れ線を、元の折れ線からの距離が指定値以下になるように簡略化する。始点と終点は必ず残り、頂点の順序も保たれる。以下、頂点数を $N$、次元を $D$ とする。

## `douglas_peucker_indices(polyline, tolerance)`

残す頂点の添字を返す。`polyline` の要素は `std::array<T, Dimension>` で、`tolerance` は非負の有限値でなければならない。点と線分の距離は、線分への射影を用いて計算する。各区間で最大距離が `tolerance` 以下になった時点で区間の内部の頂点を除く。

- 時間計算量: 最悪 $O(N^2D)$
- 追加空間計算量: $O(N)$

## `douglas_peucker(polyline, tolerance)`

残した頂点そのものを返す。

- 時間計算量: 最悪 $O(N^2D)$
- 追加空間計算量: $O(N)$（戻り値を含む）

## 注意点

- 非有限の座標、負または非有限の `tolerance` では `std::invalid_argument` を送出する。距離の計算や `tolerance` の二乗が有限の範囲に収まらない場合は `std::overflow_error` を送出する。
- 自己交差の有無や、閉曲線としての位相は保存しない。

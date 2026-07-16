---
title: Douglas–Peucker Algorithm (Douglas–Peucker法)
documentation_of: ../src/approximate/geometry/douglas_peucker.hpp
---

## Douglas–Peucker法

折れ線を、元の折れ線からの距離が指定値以下になるように簡略化する。始点と終点は必ず残り、入力順序も保存される。

## `douglas_peucker_indices(polyline, tolerance)`

残す頂点の添字を返す。`polyline` の要素は `std::array<T, Dimension>`、`tolerance` は有限な非負値でなければならない。点と線分の距離には線分への射影を用いる。

- 時間計算量: 最悪 $O(N^2D)$
- 追加空間計算量: $O(N)$
- 誤差: 各再帰区間で最大距離が `tolerance` 以下になった時点で区間内部を除く。自己交差や閉曲線としての位相は保存しない。

## `douglas_peucker(polyline, tolerance)`

残した頂点そのものを返す。

- 時間計算量: 最悪 $O(N^2D)$
- 追加空間計算量: $O(N)$（返り値を含む）

座標、距離計算、許容誤差の二乗が有限範囲に収まらない場合は例外を送出する。

## 注意点

入力座標と許容誤差は各APIの有限性・次元条件を満たし、距離・面積の中間値も表現可能でなければならない。乱択手法の結果は乱数器状態に依存する。

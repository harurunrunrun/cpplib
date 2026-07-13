---
title: OPTICS
documentation_of: ../src/approximate/clustering/optics.hpp
---

密なEuclid空間上でOPTICS orderingを構築する。`approximate::clustering` 名前空間に属する。

## OPTICSResult

- `ordering`: OPTICSが処理した点番号の順列
- `reachability_distance[v]`: 点 `v` がorderingへ追加されたときのreachability distance。各連結な探索開始点では無限大
- `core_distance[v]`: `maximum_radius` 内に自分自身を含めて `minimum_points` 個以上ある場合の `minimum_points` 番目の距離。それ以外は無限大

## optics

~~~cpp
OPTICSResult optics(
    const DensePoints<Real>& points,
    long double maximum_radius,
    size_t minimum_points
)
~~~

全点間距離を前計算し、未処理点のreachability distanceをpriority queueで更新する。同距離では点番号が小さいものを先に処理するため決定的である。cluster labelを直接返すアルゴリズムではなく、orderingとreachability plotから密度構造を取り出すための結果を返す。

点数を $N$、次元を $D$ とする。

- 時間計算量: $O(N^2D+N^2\log N)$
- 追加空間計算量: $O(N^2)$

`maximum_radius` は有限な非負値、`minimum_points` は正でなければならない。空入力を許す。座標は有限で次元が一致しなければならない。距離を `long double` で表せない場合は `std::overflow_error` を送出する。

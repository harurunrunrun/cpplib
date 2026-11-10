---
title: OPTICS (密度ベースクラスタリング順序付け)
documentation_of: ../../../src/approximate/clustering/optics.hpp
---

密なユークリッド空間上で OPTICS の順序付けを構築する。`approximate::clustering` 名前空間に属する。

## `OPTICSResult`

- `ordering`: OPTICS が処理した点番号の順列
- `reachability_distance[v]`: 点 `v` が順序に追加されたときの到達可能距離。探索の開始点では無限大
- `core_distance[v]`: `maximum_radius` 以内に自身を含めて `minimum_points` 個以上の点がある場合、`minimum_points` 番目に近い点までの距離。それ以外は無限大

## `optics`

~~~cpp
OPTICSResult optics(
    const DensePoints<Real>& points,
    long double maximum_radius,
    size_t minimum_points
)
~~~

全点間の距離を前計算し、未処理の点の到達可能距離を優先度付きキューで更新する。同距離の場合は点番号が小さいものを先に処理するため、結果は決定的である。クラスタ番号を直接返すアルゴリズムではなく、順序と到達可能距離のプロットから密度構造を取り出すための結果を返す。

点数を $N$、次元を $D$ とする。

- 時間計算量: $O(N^2D+N^2\log N)$
- 追加空間計算量: $O(N^2)$

## 注意点

- `maximum_radius` は非負の有限値、`minimum_points` は正でなければならず、満たさない場合は `std::invalid_argument` を送出する。
- 空の入力は受理する。座標は有限で、次元が一致しなければならない。
- 距離を `long double` で表せない場合は `std::overflow_error` を送出する。

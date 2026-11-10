---
title: Affinity Propagation (アフィニティ伝播法)
documentation_of: ../../../src/approximate/clustering/affinity_propagation.hpp
---

全点対のユークリッド距離の二乗に負号を付けたものを類似度として、Affinity Propagation を行う。反復的なヒューリスティックであり、指定回数内での収束や、exemplar（代表点）の一意性は保証しない。

## `AffinityPropagationResult`

- `exemplars`: exemplar となった点番号の昇順列
- `labels[i]`: 点 `i` を割り当てた exemplar の、`exemplars` 内での添字
- `iterations`: メッセージを更新した回数
- `converged`: 1 個以上の点からなる exemplar 集合が `convergence_iterations` 回連続で変化しなければ `true`

## `affinity_propagation`

~~~cpp
AffinityPropagationResult affinity_propagation(
    const DensePoints<Real>& points,
    optional<long double> preference = nullopt,
    long double damping = 0.5L,
    size_t maximum_iterations = 200,
    size_t convergence_iterations = 15
)
~~~

`preference` を省略すると、非対角成分の類似度の中央値を使う。`damping` は更新時に古いメッセージを残す比率である。同値の場合は点番号が小さい候補を選び、乱数は使わない。反復終了時に exemplar が 1 つもない場合は、対角成分の証拠値が最大の 1 点を代替として返すが、この場合はそれだけでは `converged` を `true` にしない。点が 1 個なら、その点を exemplar とし `converged = true` で返す。

点数を $N$、次元を $D$、実行した反復回数を $I$ とする。

- 時間計算量: $O(N^2D+IN^2)$
- 追加空間計算量: $O(N^2)$

## 注意点

- 点は全て同じ正の次元を持ち、座標は有限値でなければならない。空の入力は受理し、空の結果を返す。
- `preference` は指定する場合は有限値、`0 <= damping < 1`、`convergence_iterations > 0` でなければならず、満たさない場合は `std::invalid_argument` を送出する。
- $N(N-1)$ が `size_t` に収まらない場合は `std::length_error`、類似度やメッセージを `long double` で表せない場合は `std::overflow_error` を送出する。
- 入力順に依存しうるため、大域最適性は保証しない。

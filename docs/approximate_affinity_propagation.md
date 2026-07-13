---
title: Affinity Propagation (アフィニティ伝播法)
documentation_of: ../src/approximate/clustering/affinity_propagation.hpp
---

全点対の負のEuclid距離二乗をsimilarityとしてAffinity Propagationを行う。反復的なheuristicであり、指定回数内の収束や一意なexemplarを保証しない。

## AffinityPropagationResult

- `exemplars`: exemplarとなった点番号の昇順列
- `labels[i]`: `exemplars` 内で点 `i` を割り当てたindex
- `iterations`: message更新回数
- `converged`: 1個以上のexemplar集合が `convergence_iterations` 回連続で不変ならtrue

## affinity_propagation

~~~cpp
AffinityPropagationResult affinity_propagation(
    const DensePoints<Real>& points,
    optional<long double> preference = nullopt,
    long double damping = 0.5L,
    size_t maximum_iterations = 200,
    size_t convergence_iterations = 15
)
~~~

`preference` を省略するとoff-diagonal similarityの中央値を使う。`damping` は古いmessageの比率である。同値では点番号が小さい候補を選び、乱数を使わない。反復終了時にexemplarがない場合は対角evidence最大の1点をfallbackとして返すが、この場合だけでは `converged` をtrueにしない。

点数を $N$、次元を $D$、実行iteration数を $I$ とする。

- 時間計算量: $O(N^2D+IN^2)$
- 追加空間計算量: $O(N^2)$

`preference` は指定時に有限、`0 <= damping < 1`、`convergence_iterations > 0` を要求する。空入力を許す。similarityまたはmessageを表せない場合は `std::overflow_error` を送出する。

## 注意点

座標列または距離callbackは、各APIで示した次元・有限性・非負性の条件を満たす必要がある。初期値や入力順に依存する手法では、明記した場合を除いて一意な分割や大域最適性を保証しない。

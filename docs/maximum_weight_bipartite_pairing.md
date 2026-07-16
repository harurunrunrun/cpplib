---
title: Maximum-Weight Bipartite Pairing (最大重み二部対応) [SCITIES]
documentation_of: ../src/algorithm/matching/maximum_weight_bipartite_pairing.hpp
---

左右の各要素を高々1回ずつ使用する対応について、重み和の最大値を返す。

## MaximumWeightBipartitePairingEdge

```cpp
struct MaximumWeightBipartitePairingEdge {
    int left;
    int right;
    long long weight;
};
```

左側の添字、右側の添字、対応させたときの非負重みを表す。添字は0始まり。

## maximum_weight_bipartite_pairing

```cpp
long long maximum_weight_bipartite_pairing(
    int left_size,
    int right_size,
    const vector<MaximumWeightBipartitePairingEdge>& edges
);
```

指定された辺だけを候補とする最大重み対応の重み和を返す。要素を対応させずに残すことができる。同じ端点対が複数回指定された場合は最大の重みを使用する。

## 時間計算量

$N=\max(L,R)$、$E=\lvert\mathrm{edges}\rvert$ とする。

- 入力検査と重み表の構築: $O(N^2+E)$
- `maximum_weight_bipartite_pairing`: $O(N^3+E)$

## 空間計算量

- 重み表と作業領域: $O(N^2)$

## 注意点

- `left_size` と `right_size` は非負でなければならない。
- 各端点は対応する添字範囲内、重みは非負でなければならない。違反時は `runtime_error` を送出する。
- 答えが `long long` に収まらない場合は `overflow_error` を送出する。

---
title: Pairwise Disconnect Cost Sum (全頂点対切断コスト総和) [KOICOST]
documentation_of: ../src/algorithm/graph/connectivity/pairwise_disconnect_cost_sum.hpp
---

重みが相異なる無向グラフについて、辺を重みの昇順に削除したときの全頂点対の切断コスト総和を求める。

頂点対 $(u,v)$ が最初から非連結ならコストは0とする。
連結なら、$u,v$ が非連結になるまでに削除した全辺の重みの総和をその頂点対のコストとする。

## PairwiseDisconnectCostEdge

```cpp
template<class T>
using PairwiseDisconnectCostEdge = KruskalEdge<T>;
```

`PairwiseDisconnectCostEdge<T>{from, to, cost}` で無向辺を表す。

## pairwise_disconnect_cost_sum

```cpp
template<class T>
long long pairwise_disconnect_cost_sum(
    int n,
    const vector<PairwiseDisconnectCostEdge<T>>& edges,
    long long modulus
)
```

すべての $0\le u<v<n$ に対する切断コストの総和を `modulus` で割った余りで返す。


## API別の時間計算量・空間計算量

$N=n$、$M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_disconnect_cost_sum(n, edges, modulus)` | $O(M\log M+N)$ | $O(N+M)$ |

## 注意点

- `n` は非負、頂点番号は $[0,n)$、辺重みは非負、`modulus` は正でなければならない。違反時は `runtime_error` を送出する。
- 辺重みはすべて相異なる必要があり、重複時は `runtime_error` を送出する。
- `T` は整数型でなければならない。
- 自己辺と多重辺を入力できる。自己辺も、削除される時点以降の接頭和を通して他の頂点対のコストに影響し得る。
- 異なる連結成分に属し続ける頂点対は総和に寄与しない。
- 積は内部で128-bit整数を用いてから剰余を取る。

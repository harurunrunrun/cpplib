---
title: Pairwise Disconnect Cost Sum (全頂点対切断コスト総和) [KOICOST]
documentation_of: ../../../../../src/algorithm/graph/connectivity/resilience/pairwise_disconnect_cost_sum.hpp
---

重みが相異なる無向グラフについて、辺を重みの昇順に削除していくときの、全頂点対の切断コストの総和を求める。

頂点対 $(u, v)$ が最初から非連結ならコストは 0 とする。連結なら、$u$ と $v$ が非連結になるまでに削除した全辺の重みの総和を、その頂点対のコストとする。

辺を重みの降順に Union-Find へ追加し、2 成分が併合されるとき、その辺を削除した時点で切れる頂点対の個数と、それまでに削除した辺の重みの和（接頭辞和）との積を加算する。

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

$0 \le u \lt v \lt n$ を満たすすべての頂点対の切断コストの総和を、`modulus` で割った余りとして返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_disconnect_cost_sum(n, edges, modulus)` | $O(M \log M + N)$ | $O(N + M)$ |

## 注意点

- `n` は非負、頂点番号は $[0, n)$、辺の重みは非負、`modulus` は正でなければならない。違反時は `std::runtime_error` を送出する。
- 辺の重みはすべて相異なる必要があり、重複している場合は `std::runtime_error` を送出する。
- `T` は整数型でなければならない（`static_assert` で検査する）。
- 自己ループと多重辺を入力できる。自己ループの重みも、それより後に切断される頂点対のコスト（それまでに削除した辺の重みの和）に含まれる。
- 最後まで異なる連結成分に属する頂点対は総和に寄与しない。
- 積は内部で 128 ビット整数を用いて計算してから剰余を取る。

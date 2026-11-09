---
title: K-th Shortest Path (K番目最短路)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/enumeration/kth_shortest_path.hpp
---

非負重みの有向グラフで、`source` から `target` への単純パスを距離の小さい順に列挙する（Yen のアルゴリズム）。

同じ頂点列は 1 つのパスとして扱う（多重辺があっても、頂点列が同じなら区別しない）。

## 関数

```cpp
template <class T>
std::vector<KthShortestPathResult<T>> kth_shortest_paths(
    const std::vector<std::vector<KthShortestPathEdge<T>>>& graph,
    int source,
    int target,
    int k,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`graph[v]` は `KthShortestPathEdge<T>{to, cost}` の列で、頂点 `v` から出る辺を表す。

戻り値は距離の小さい順に並んだ高々 `k` 個の `KthShortestPathResult<T>` であり、各要素は次のメンバをもつ。

```cpp
T cost;
std::vector<int> vertices;
std::vector<T> prefix_cost;
```

- `cost`: パスの距離。
- `vertices`: パスの頂点列（両端を含む）。
- `prefix_cost[i]`: `source` から `vertices[i]` までの距離。`vertices` と同じ長さである。

`inf` は内部の初期値にすぎず、到達可能なパスの距離が `inf` 以上でも正しく扱える。

## 時間計算量

受理済みのパスは頂点列の接頭辞トライで管理し、各スパー（分岐点）で受理済みの全パスと比較することはしない。

受理したパスの長さの総和を $P$、生成した候補パスの個数を $Q$ とする。$P, Q \le KN$ である。

- 最短路探索: $O(P(M + N)\log(M + N + 1))$
- 接頭辞トライの構築・走査と禁止辺の判定: $O((P + KM)\log(P + 1))$
- 候補のヒープと重複除外の集合: $O(QN\log(Q + 1))$
- 全体: $O(P(M + N)\log(M + N + 1) + (P + KM)\log(P + 1) + QN\log(Q + 1))$

禁止する次の頂点のトライ探索は、スパー頂点から出る辺についてだけ行う。パスは単純なので、全受理パス・全スパーにわたる対象の辺の本数は $O(KM)$ である。戻り値の要素数の総和は $P$ である。

## 空間計算量

- 接頭辞トライ: $O(P)$
- 戻り値、候補パス、重複除外の集合を含めて $O(P + QN)$

## 注意点

- `source`, `target` と辺の行き先は $[0, N)$ の範囲内、`k` と辺の重みは非負でなければならない。違反時は `runtime_error` を送出する。
- `source == target` のときは長さ 0 のパス 1 本だけを返す（`k >= 1` の場合）。
- パスの距離と辺の重みの加算結果は `T` で表現できなければならない。

---
title: K-th Shortest Path (K番目最短路)
documentation_of: ../src/algorithm/graph/kth_shortest_path.hpp
---

非負重み有向グラフで、単純パスを距離の小さい順に列挙する。

同じ頂点列は1つのパスとして扱う。

# 関数

```cpp
kth_shortest_paths<T>(graph, source, target, k, inf)
```

`graph[v]` は `KthShortestPathEdge<T>{to, cost}` の列。

戻り値は高々 `k` 個の `KthShortestPathResult<T>`。

```cpp
T cost;
vector<int> vertices;
vector<T> prefix_cost;
```

`prefix_cost[i]` は `source` から `vertices[i]` までの距離。

`inf` は内部の初期値であり、到達可能なパスの距離が `inf` 以上でも扱える。

パスの距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

Yen法。受理済みpathは頂点列のprefix trieで管理し、各spurで全受理済みpathを比較しない。

受理したpath長の総和を $P$、生成した候補path数を $Q$ とする。$P,Q\leq KN$。

- `kth_shortest_paths` 内のshortest path探索: $O(P(M+N)\log(N+1))$
- prefix trieの構築・走査と禁止辺判定: $O((P+KM)\log(P+1))$
- 候補heapと重複除外集合: $O(QN\log(Q+1))$
- 全体: $O(P(M+N)\log(N+1)+(P+KM)\log(P+1)+QN\log(Q+1))$

禁止nextのtrie探索はspur頂点から出る辺だけで行う。pathは単純なので、全受理path・全spurにわたる対象辺数は $O(KM)$。戻り値の出力要素数は $P$。

## 空間計算量

- prefix trieは $O(P)$
- 戻り値、候補path、重複除外集合を含めて $O(P+QN)$

## API契約・前提・例外

`source`, `target`, 辺の行き先は頂点範囲内、`k` と辺costは非負とする。戻り値は高々 `k` 個の単純pathで、各 `vertices` と `prefix_cost` の長さは等しい。違反時は `runtime_error`。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。

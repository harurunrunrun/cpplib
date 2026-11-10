---
title: Bipartite b-Matching (二部b-マッチング)
documentation_of: ../../../../src/algorithm/matching/bipartite/bipartite_b_matching.hpp
---

左右の各頂点と各辺に非負の容量を持つ二部グラフで、選ぶ量の総和が最大となる整数 $b$-マッチングを求める。

## API

```cpp
BipartiteBMatching<Capacity> matching(left_capacity, right_capacity);
```

左右の頂点容量（`vector<Capacity>`）を指定して構築する。頂点数はそれぞれの長さで決まる。`Capacity` のデフォルトは `long long`。

```cpp
int edge_id = matching.add_edge(left, right, capacity = 1);
```

容量付きの辺を追加し、その番号を返す。

```cpp
BipartiteBMatchingResult<Capacity> result = matching.solve(limit);
```

総流量を `limit` 以下の範囲で最大化する。結果は総量 `size`、入力順の各辺の流量 `edge_flow`、左右の各頂点の使用量 `left_degree`, `right_degree` を持つ。`limit` のデフォルトは `numeric_limits<Capacity>::max() / 4` で、実質的に無制限である。

## 時間計算量

左右の頂点数の和を $V$、辺数を $E$ とする。

- 構築: $O(V)$
- `add_edge`: 償却 $O(1)$
- `solve`: Dinic 法により一般には $O(V^2 E)$

単位容量だけからなる通常の二部マッチングには、専用の `BipartiteMatching` の方が高速である。

## 空間計算量

$O(V + E)$。

## 注意点

容量は `Capacity` で正確に表現できる非負整数とする。頂点番号の範囲外、負の容量、負の `limit` に対しては `runtime_error` を送出する。

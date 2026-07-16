---
title: Connected Components of Complement Graph (補グラフの連結成分) [connected_components_of_complement_graph]
documentation_of: ../src/algorithm/graph/connected_components_of_complement_graph.hpp
---

無向グラフの補グラフを連結成分分解する。未所属頂点を保持し、BFS中の頂点と元のグラフで隣接しない頂点を同じ補グラフ成分へ移す。

## API

```cpp
ComplementConnectedComponentsResult
connected_components_of_complement_graph(
    const vector<vector<int>>& graph
)
```

`graph[v]`には元の無向グラフで`v`に隣接する頂点を格納する。

返り値は次を持つ。

```cpp
vector<int> component_id;
vector<vector<int>> groups;
```

- `component_id[v]`: 頂点`v`が属する成分番号
- `groups[id]`: 成分`id`の頂点列

空グラフに対しては両方とも空になる。隣接頂点番号が範囲外なら例外を送出する。

## 時間計算量

- $O(N+M)$

未所属頂点を走査したとき、補グラフ辺なら頂点を一度だけ取り除き、元のグラフ辺ならその辺に走査を対応させる。

## 空間計算量

- 入力を除いて $O(N)$

## 注意点

頂点番号は $[0,N)$ に収め、辺の向き、多重辺、自己loop、連結性は各APIで記した入力条件に従う。戻り値の頂点番号も同じ0-indexedである。

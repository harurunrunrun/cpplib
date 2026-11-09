---
title: Connected Components of Complement Graph (補グラフの連結成分) [connected_components_of_complement_graph]
documentation_of: ../../../../../src/algorithm/graph/connectivity/components/connected_components_of_complement_graph.hpp
---

無向グラフの補グラフを連結成分に分解する。まだどの成分にも属していない頂点の集合を保持し、BFS で取り出した頂点と元のグラフで隣接しない未所属頂点を、同じ補グラフ成分へ移していく。

## API

```cpp
ComplementConnectedComponentsResult
connected_components_of_complement_graph(
    const vector<vector<int>>& graph
)
```

`graph[v]` には、元の無向グラフで `v` に隣接する頂点を格納する。

戻り値は次のメンバを持つ。

```cpp
vector<int> component_id;
vector<vector<int>> groups;
```

- `component_id[v]`: 頂点 `v` が属する成分番号
- `groups[id]`: 成分 `id` の頂点列

頂点数 0 のグラフに対しては、どちらも空になる。

## 時間計算量

頂点数を $N$、隣接リストの総要素数を $M$ とする。

- $O(N + M)$

未所属頂点を走査したとき、補グラフの辺で結ばれていればその頂点を一度だけ取り除き、元のグラフの辺で結ばれていればその走査を元のグラフの辺に対応させられるため、走査の総量は $O(N + M)$ に収まる。

## 空間計算量

- 入力を除いて $O(N)$

## 注意点

- 隣接頂点の番号は $[0, N)$ でなければならず、範囲外なら `std::runtime_error` を送出する。
- 隣接関係は両方向に格納すること。自己ループと多重辺があっても結果は変わらない。
- 戻り値の頂点番号も入力と同じ 0-indexed である。

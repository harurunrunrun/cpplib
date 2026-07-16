---
title: Radius Balls Form Vertex Partition (距離球による頂点分割判定) [AKBAR]
documentation_of: ../src/algorithm/graph/radius_balls_form_vertex_partition.hpp
---

無向グラフ上の複数の閉距離球が、頂点集合を重複なく過不足なく分割するかを
判定します。

## API

### `RadiusBall`

```cpp
struct RadiusBall {
    int center;
    int radius;
};
```

中心 `center` と非負半径 `radius` を持つ閉距離球を表します。
この球には、中心からの最短路距離が半径以下の頂点が含まれます。

### `radius_balls_form_vertex_partition(vertex_count, undirected_edges, balls)`

```cpp
bool radius_balls_form_vertex_partition(
    int vertex_count,
    const vector<pair<int, int>>& undirected_edges,
    const vector<RadiusBall>& balls
);
```

全頂点がちょうど1個の球に含まれるなら `true` を返します。
未被覆頂点または複数の球に属する頂点があれば `false` です。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、球数を $B$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `RadiusBall` の構築 | $O(1)$ | $O(1)$ |
| `radius_balls_form_vertex_partition` | $O(N+M+B)$ | $O(N+M+B)$ |

## 注意点

- `vertex_count` と各半径は非負でなければなりません。
- 辺端点と球中心は $[0,N)$ に入る必要があります。
- 多重辺と自己loopを入力できます。
- 前提違反には `invalid_argument`、`out_of_range` または
  `length_error` を送出します。

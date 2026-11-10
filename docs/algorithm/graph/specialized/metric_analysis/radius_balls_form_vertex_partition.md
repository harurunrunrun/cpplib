---
title: Radius Balls Form Vertex Partition (距離球による頂点分割判定) [AKBAR]
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/radius_balls_form_vertex_partition.hpp
---

無向グラフ上の複数の閉距離球が、頂点集合を重複も漏れもなく分割するかを判定する。

## API

### `RadiusBall`

```cpp
struct RadiusBall {
    int center;
    int radius;
};
```

中心 `center` と非負の半径 `radius` を持つ閉距離球を表す。
この球には、中心からの最短路長（辺数）が半径以下の頂点が含まれる。

### `radius_balls_form_vertex_partition(vertex_count, undirected_edges, balls)`

```cpp
bool radius_balls_form_vertex_partition(
    int vertex_count,
    const vector<pair<int, int>>& undirected_edges,
    const vector<RadiusBall>& balls
);
```

全頂点がちょうど 1 個の球に含まれるなら `true` を返す。
どの球にも含まれない頂点、または複数の球に属する頂点があれば `false` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、球の個数を $B$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `RadiusBall` の構築 | $O(1)$ | $O(1)$ |
| `radius_balls_form_vertex_partition` | $O(N+M+B)$ | $O(N+M+B)$ |

## 注意点

- `vertex_count` と各半径は非負でなければならない。
- 辺の端点と球の中心は $[0,N)$ に入る必要がある。
- 多重辺と自己ループを入力できる。
- 前提違反には `invalid_argument`、`out_of_range` または `length_error` を送出する。ただし球は先頭から順に検査し、2 つの球の中心が重なった時点で `false` を返すため、それより後ろの球の前提違反は検出されないことがある。

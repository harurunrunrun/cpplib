---
title: Visibility Graph Aggregator (可視グラフ集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/visibility_graph.hpp
---

可視グラフの辺型と多角形入力の API をまとめて読み込む、後方互換のための集約ヘッダ。

## 構成

| 個別ヘッダ | 提供する API |
| --- | --- |
| `visibility_graph_edge.hpp` | `VisibilityGraphEdge` |
| `visibility_graph_points.hpp` | `visibility_graph(polygon)` |

必要な個別ヘッダだけを直接インクルードしてもよい。従来どおり `visibility_graph.hpp` をインクルードすれば全 API を利用できる。

## 集約される API

```cpp
std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
);
```

単純多角形の頂点同士を、内部または境界だけを通る線分で結んだ無向可視グラフを返す。
各辺 `VisibilityGraphEdge` は、小さい方の入力添字 `first`、大きい方の入力添字 `second`、2 頂点間のユークリッド距離 `distance` を持つ。

## API別の時間計算量・空間計算量

頂点数を $N$、返す辺数を $K$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `visibility_graph(polygon)` | $O(N^2 \log N + K)$ | $O(N^2)$ |
| `VisibilityGraphEdge` の構築・各メンバの参照 | $O(1)$ | $O(1)$ |

$K = O(N^2)$ なので、時間計算量の上界は $O(N^2 \log N)$ である。

## 注意点

- 入力は 3 頂点以上で、座標が有限、面積が 0 でない単純多角形でなければならない。
- 退化辺や自己交差があれば `std::invalid_argument` を送出する。
- 平面走査のための座標変換や内部サイズを安全に計算できない場合は、`std::overflow_error` または `std::length_error` を送出することがある。
- 開線分の内部を第 3 の頂点が通る頂点対は、直接の辺に含めない。

---
title: Visibility Graph Aggregator (可視グラフ集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/visibility_graph.hpp
---

可視グラフの辺型と多角形入力APIをまとめて読み込む後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `visibility_graph_edge.hpp` | `VisibilityGraphEdge` |
| `visibility_graph_points.hpp` | `visibility_graph(polygon)` |

必要なleafだけを直接includeできる。従来の `visibility_graph.hpp` も
引き続き全APIを提供する。

## 集約されるAPI

```cpp
std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
);
```

単純多角形の頂点間を内部または境界だけを通る線分で結んだ無向可視グラフを返す。
各辺は小さい入力添字、大きい入力添字、2頂点間のEuclid距離を保持する。

## API別の時間計算量・空間計算量

頂点数を $N$、返す辺数を $K$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `visibility_graph(polygon)` | $O(N^2\log N + K)$ | $O(N^2)$ |
| `VisibilityGraphEdge` の構築・各field参照 | $O(1)$ | $O(1)$ |

$K=O(N^2)$ なので時間の上界は $O(N^2\log N)$ である。

## 注意点

- 入力は3頂点以上、有限座標、面積非零の単純多角形でなければならない。
- 退化辺または自己交差には `std::invalid_argument` を送出する。
- sweepの変換や内部サイズを安全に計算できない場合は `std::overflow_error` または `std::length_error` を送出し得る。
- 第3頂点を開線分上に通る頂点対は直接辺に含めない。

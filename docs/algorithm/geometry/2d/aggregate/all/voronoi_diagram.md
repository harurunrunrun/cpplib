---
title: 2D Voronoi Diagram Aggregator (二次元 Voronoi 図集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/voronoi_diagram.hpp
---

Voronoi 辺の種類、辺、図全体の結果型と、点列を入力とする API をまとめて読み込む、後方互換のための集約ヘッダ。

## 構成

| 個別ヘッダ | 提供する API |
| --- | --- |
| `voronoi_edge_kind.hpp` | `VoronoiEdgeKind` |
| `voronoi_edge.hpp` | `VORONOI_NO_VERTEX`, `VoronoiEdge` |
| `voronoi_diagram_result.hpp` | `VoronoiDiagramResult` |
| `voronoi_diagram_points.hpp` | `voronoi_diagram(points)` |

必要な個別ヘッダだけを直接インクルードしてもよい。従来どおり `voronoi_diagram.hpp` をインクルードすれば全 API を利用できる。

## 集約される API

```cpp
VoronoiDiagramResult voronoi_diagram(const std::vector<Point>& points);
```

入力点を母点とする Voronoi 図を返す。

## `VoronoiEdge`

- `kind`: `VoronoiEdgeKind::SEGMENT`、`RAY`、`LINE` のいずれか。
- `first_site`, `second_site`: 辺を共有する 2 つの母点の、元の入力添字。
- `origin`: 線分の一端、半直線の始点、または直線上の 1 点。
- `endpoint_or_direction`: `SEGMENT` では他端、`RAY` と `LINE` では長さ 1 の方向ベクトル。
- `first_vertex`, `second_vertex`: `vertices` への添字。有限の端点がない側は `VORONOI_NO_VERTEX`（`std::size_t` の最大値）。

有限の端点は `SEGMENT` が 2 つ、`RAY` が 1 つ、`LINE` が 0 個持つ。

## `VoronoiDiagramResult`

- `vertices`: Voronoi 頂点。
- `edges`: Voronoi 辺。
- `cell_edges[i]`: 入力点 `i` のセル境界を構成する辺の添字。順序は規定しない。
- `representative`: 各入力点に対応する母点の入力添字。座標の辞書順に走査して先に選ばれたものが代表になる。
- `sites`: 重複を除いた母点の代表添字。座標の辞書順に並ぶ。

代表でない重複点の `cell_edges` は空であり、そのセルは `representative[i]` のセルを参照すればよい。

## API別の時間計算量・空間計算量

点数を $N$、`cell_edges[i]` の長さを $d_i$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `voronoi_diagram(points)` | $O(N \log N)$ | $O(N)$ |
| `result.vertices` / `result.edges` の全列挙 | $O(N)$ | $O(1)$ |
| `result.cell_edges[i]` の全列挙 | $O(d_i)$ | $O(1)$ |
| `result.representative[i]` / `result.sites[i]` | $O(1)$ | $O(1)$ |

## 注意点

- NaN または無限大の座標には `std::invalid_argument` を送出する。Delaunay 外接円判定の中間値や Voronoi 頂点が有限範囲を超える場合は `std::overflow_error` を送出する。
- Delaunay 三角形分割が非共線と判定した三角形には、同じ丸め誤差基準で外心を求める。非常に細い三角形も扱うが、外心が `long double` の有限範囲を超える場合は `std::overflow_error` を送出する。
- 点が 0 個、または母点が 1 種類しかない場合は、頂点も辺も返さない。
- 母点が 2 種類の場合、および全母点が同一直線上にある場合は、隣接する母点間の垂直二等分線を `LINE` として返す。
- 非退化な凸包に対応する無限辺は `RAY` として返す。
- 4 点以上が同一円周上にある場合は、共有 Delaunay 辺の両側の 4 点を Delaunay と同じ相対丸め誤差付きの外接円述語で判定し、同じ Voronoi 頂点に対応する三角形を統合する。入力座標を大きく拡大しても、数値誤差だけで生じた長さ 0 の双対辺は返さない。
- `cell_edges` は境界辺の巡回順ではない。
- `GEOMETRY_EPS` で近接する母点の代表の選び方は Delaunay 三角形分割と同じであり、入力順で最小の添字になるとは限らない。

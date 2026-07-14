---
title: 2D Voronoi Diagram (二次元Voronoi図)
documentation_of: ../src/algorithm/geometry/2d/voronoi_diagram.hpp
---

## API

```cpp
VoronoiDiagramResult voronoi_diagram(const std::vector<Point>& points);
```

入力点をsiteとするVoronoi図を返す。

## `VoronoiEdge`

- `kind`: `VoronoiEdgeKind::SEGMENT`, `RAY`, `LINE` のいずれか。
- `first_site`, `second_site`: 辺を共有する2つのsiteの元入力添字。
- `origin`: 線分の一端、半直線の始点、または直線上の点。
- `endpoint_or_direction`: `SEGMENT`では他端、`RAY`と`LINE`では長さ1の方向ベクトル。
- `first_vertex`, `second_vertex`: `vertices`への添字。有限端点がない箇所は `VORONOI_NO_VERTEX`。

`SEGMENT`は2つ、`RAY`は1つ、`LINE`は0個の有限端点を持つ。

## `VoronoiDiagramResult`

- `vertices`: Voronoi頂点。
- `edges`: Voronoi辺。
- `cell_edges[i]`: 入力点 `i` のcell境界を構成する辺の添字。順序は規定しない。
- `representative`: 各入力点に対応する、座標辞書順の走査で先に選ばれたsiteの入力添字。
- `sites`: 重複を除いたsiteの代表添字。座標の辞書順である。

代表でない重複点の `cell_edges` は空である。`representative[i]` のcellを参照する。

## API別の時間計算量・空間計算量

点数を $N$、返す辺数を $E$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `voronoi_diagram(points)` | $O(N\log N)$ | $O(N)$ |
| `result.vertices` / `result.edges` の全列挙 | $O(N)$ | $O(1)$ |
| `result.cell_edges[i]` の全列挙 | $O(|cell_edges[i]|)$ | $O(1)$ |
| `result.representative[i]` / `result.sites[i]` | $O(1)$ | $O(1)$ |

## 注意点

- NaNまたは無限大の座標には `std::invalid_argument` を送出する。Delaunay外接円判定の中間値、またはVoronoi頂点が有限範囲を超える場合は `std::overflow_error` を送出する。
- Delaunay三角形分割が非共線と判定した三角形には同じ丸め誤差基準を用いて外心を構成する。非常に細い三角形も処理するが、外心が `long double` の有限範囲を超える場合は `std::overflow_error` を送出する。
- 0点または1種類のsiteでは頂点と辺を返さない。
- 2種類のsite、および全siteが同一直線上の場合、隣接site間の垂直二等分線を `LINE` として返す。
- 非退化な凸包に対応する無限辺は `RAY` として返す。
- 4点以上が同一円周上にある場合、共有Delaunay辺の両側4点をDelaunayと同じ相対丸め誤差付き外接円述語で判定し、同じVoronoi頂点へ連結される三角形を統合する。入力座標を大きく拡大した場合も、数値誤差だけで生じた長さ0の双対辺を返さない。
- `cell_edges` は境界辺の巡回順ではない。
- EPS近接siteの代表選択はDelaunay三角形分割と同じで、入力順の最小添字とは限らない。

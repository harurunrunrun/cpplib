---
title: Voronoi Diagram Result (Voronoi図の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/voronoi_diagram_result.hpp
---

Voronoi図と重複siteの対応を保持する結果型。

## API

```cpp
struct VoronoiDiagramResult {
    std::vector<Point> vertices;
    std::vector<VoronoiEdge> edges;
    std::vector<std::vector<std::size_t>> cell_edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};
```

- `vertices`: 有限なVoronoi頂点。
- `edges`: Voronoi辺。
- `cell_edges[i]`: 入力点 `i` のcell境界を構成する辺の添字。順序は未規定。
- `representative[i]`: 入力点 `i` と同一siteとして扱う代表の入力添字。
- `sites`: 重複を除いたsiteの代表添字を座標辞書順に並べた列。

既定構築では全fieldが空となる。

## 時間計算量

頂点数を $V$、辺数を $E$、入力点数を $N$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、各fieldの参照、`size()` | $O(1)$ | $O(1)$ |
| `vertices` の全列挙 | $O(V)$ | $O(1)$ |
| `edges` の全列挙 | $O(E)$ | $O(1)$ |
| `cell_edges[i]` の全列挙 | $O(\lvert cell\_edges[i]\rvert)$ | $O(1)$ |
| `representative` / `sites` の全列挙 | $O(N)$ | $O(1)$ |

## 注意点

代表でない重複点の `cell_edges` は空である。そのcellは
`representative[i]` が指す代表siteのcellとして参照する。

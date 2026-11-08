---
title: Voronoi Diagram Result (Voronoi 図の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/voronoi_diagram_result.hpp
---

Voronoi 図と、重複したサイトの対応関係を保持する結果型。

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

- `vertices`: 有限な Voronoi 頂点。
- `edges`: Voronoi 辺。
- `cell_edges[i]`: 入力点 `i` のセルの境界を構成する辺の、`edges` における添字。順序は未規定。
- `representative[i]`: 入力点 `i` と同一のサイトとして扱われる点のうち、代表となる点の入力添字。
- `sites`: 重複を除いた各サイトの代表添字を、座標の辞書順に並べた列。

デフォルト構築ではすべてのメンバが空になる。

## 時間計算量

頂点数を $V$、辺数を $E$、入力点数を $N$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| デフォルト構築、各メンバの参照、`size()` | $O(1)$ | $O(1)$ |
| `vertices` の全列挙 | $O(V)$ | $O(1)$ |
| `edges` の全列挙 | $O(E)$ | $O(1)$ |
| `cell_edges[i]` の全列挙 | $O(\lvert C_i \rvert)$（$C_i$ は `cell_edges[i]`） | $O(1)$ |
| `representative` / `sites` の全列挙 | $O(N)$ | $O(1)$ |

## 注意点

代表でない重複点の `cell_edges` は空である。そのセルは `representative[i]` が指す代表サイトのセルとして参照する。

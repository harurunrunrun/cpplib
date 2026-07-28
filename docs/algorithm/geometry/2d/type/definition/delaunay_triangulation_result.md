---
title: Delaunay Triangulation Result (Delaunay三角形分割の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/delaunay_triangulation_result.hpp
---

Delaunay三角形分割と重複点の対応を保持する結果型。

## API

```cpp
struct DelaunayTriangulationResult {
    std::vector<std::array<std::size_t, 3>> triangles;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};
```

- `triangles`: 反時計回りの三角形。先頭は3添字中の最小値。
- `edges`: 無向辺。各組は昇順で、全体は辞書順。
- `representative[i]`: 入力点 `i` と同一siteとして扱う代表の入力添字。
- `sites`: 重複を除いたsiteの代表添字を座標辞書順に並べた列。

既定構築では全fieldが空となる。

## 時間計算量

結果の三角形数を $T$、辺数を $E$、入力点数を $N$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、各fieldの参照、`size()` | $O(1)$ | $O(1)$ |
| `triangles` の全列挙 | $O(T)$ | $O(1)$ |
| `edges` の全列挙 | $O(E)$ | $O(1)$ |
| `representative` / `sites` の全列挙 | $O(N)$ | $O(1)$ |

平面Delaunay三角形分割では $T,E=O(N)$ である。

## 注意点

すべての添字は元の入力点列に対する。代表でない重複点は三角形と辺に現れず、
`representative` を通じて対応する。4点以上が同一円周上なら結果は一意とは限らない。

---
title: 2D Delaunay Triangulation (二次元Delaunay三角形分割)
documentation_of: ../src/algorithm/geometry/2d/shape/delaunay_triangulation.hpp
---

## API

```cpp
DelaunayTriangulationResult delaunay_triangulation(
    const std::vector<Point>& points
);
```

入力点のDelaunay三角形分割を返す。

## `DelaunayTriangulationResult`

- `triangles`: 反時計回りの三角形。各頂点は元入力の添字である。
- `edges`: 三角形分割の無向辺。各組は昇順で、配列全体も辞書順である。
- `representative`: 各入力点に対応する、座標辞書順の走査で先に選ばれたsiteの入力添字。
- `sites`: 重複を除いた点の代表添字。座標の辞書順である。

`triangles`の各三角形は、先頭に最小添字を置くよう巡回移動している。

## API別の時間計算量・空間計算量

点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `delaunay_triangulation(points)` | $O(N\log N)$ | $O(N)$ |
| `result.triangles` / `result.edges` の全列挙 | $O(N)$ | $O(1)$ |
| `result.representative[i]` / `result.sites[i]` | $O(1)$ | $O(1)$ |

## 注意点

- NaNまたは無限大の座標には `std::invalid_argument` を送出する。
- 外接円判定の4次式を `long double` で有限に計算できない大きさの座標には `std::overflow_error` を送出する。
- 点の同一性は `GEOMETRY_EPS`、向きと外接円の符号は各式の項の大きさに対する `long double` の丸め誤差を考慮して判定する。
- 0点または1種類の点では辺と三角形を返さない。
- 全点が同一直線上なら、座標順で隣接する点の辺だけを返し、三角形は空である。
- 4点以上が同一円周上にある場合、Delaunay三角形分割は一意でない。このAPIはそのうち決定的に選んだ1つを返す。
- 重複点は三角形分割から除き、`representative`で対応を保持する。
- EPS近接点の等値関係は推移的とは限らないため、座標辞書順に走査し、既に選ばれた近接siteのうち最初のものへ対応付ける。入力順の最小添字とは限らない。

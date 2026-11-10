---
title: 2D Delaunay Triangulation Aggregator (二次元Delaunay三角形分割集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/delaunay_triangulation.hpp
---

公開の結果型と、点列を入力とする API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `delaunay_triangulation_result.hpp` | `DelaunayTriangulationResult` |
| `delaunay_triangulation_points.hpp` | `delaunay_triangulation(points)` |

必要な leaf header だけを直接 include することもできる。従来どおり `delaunay_triangulation.hpp` を include すれば全 API を使える。

## 集約される API

```cpp
DelaunayTriangulationResult delaunay_triangulation(const std::vector<Point>& points);
```

入力点の Delaunay 三角形分割を返す。

## `DelaunayTriangulationResult`

- `triangles`: 反時計回りの三角形の列。各頂点は元の入力の添字である。各三角形は最小の添字が先頭に来るよう巡回シフトしてあり、列全体はソートされている。
- `edges`: 三角形分割の無向辺の列。各組は昇順で、列全体も辞書順に並ぶ。
- `representative`: 各入力点に対応する site の入力添字。座標の辞書順に走査したときに先に選ばれた site である。
- `sites`: 重複を除いた点の代表の添字の列。座標の辞書順に並ぶ。

## 計算量

点の個数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `delaunay_triangulation(points)` | $O(N\log N)$ | $O(N)$ |
| `result.triangles` / `result.edges` の全列挙 | $O(N)$ | $O(1)$ |
| `result.representative[i]` / `result.sites[i]` | $O(1)$ | $O(1)$ |

## 注意点

- NaN または無限大の座標には `std::invalid_argument` を送出する。
- 外接円判定の 4 次式を `long double` で有限に計算できないほど大きい座標には `std::overflow_error` を送出する。
- 点の同一性は `GEOMETRY_EPS` で判定し、向きと外接円判定の符号は、各式の項の大きさに対する `long double` の丸め誤差を考慮して判定する。
- 点が 0 個、または 1 種類しかない場合は、辺も三角形も返さない。
- 全点が同一直線上にあれば、座標順で隣り合う点を結ぶ辺だけを返し、三角形は空になる。
- 4 点以上が同一円周上にある場合、Delaunay 三角形分割は一意でない。この API はそのうち決定的に選んだ 1 つを返す。
- 重複点は三角形分割から除き、`representative` で対応を保持する。
- EPS で近い点どうしの等値関係は推移的とは限らないため、座標の辞書順に走査し、既に選ばれた近い site のうち最初のものに対応付ける。これは入力順で最小の添字とは限らない。

---
title: Polygon Boundary Points (多角形の辺上の点列挙)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/polygon_boundary_points.hpp
---

与えた点のうち、多角形の境界上にあるものを列挙する。

## 関数

```cpp
std::vector<int> enumerate_points_on_polygon_boundary(
    const std::vector<Point>& polygon, const std::vector<Point>& points, bool unique = true);

PolygonBoundaryPointQuery query(polygon);
query.uses_convex_fast_path();
query.polygon();
query.enumerate(points, unique);
```

返り値は `points` の添字の列である。多角形の辺は頂点の順に処理し、1 本の辺の上にある点は、その辺の始点から終点に向かう順に並べる。長さ $0$ の辺も扱える。

`unique == true` なら、各点の添字は最初に現れる位置にだけ含まれる。`unique == false` なら、多角形の頂点上にある点は、その頂点に接する辺ごとに 1 回ずつ現れうる。

`PolygonBoundaryPointQuery` も、辺の順序、各辺の中での順序、`unique` の挙動を同じに保つ。退化していない凸多角形に対しては、時計回り・反時計回りの弱凸な境界を正規化し、正規化後の各辺を元の辺に対応付けて、凸多角形向けの高速な経路を使う。連続する重複頂点も扱える。

凸性・非退化性・元の辺との対応付けのいずれかを確認できない場合は `uses_convex_fast_path()` が `false` になり、`enumerate` は一般の関数を呼ぶ。凸でない多角形や全頂点が同一直線上にある多角形もこの経路で処理する。また、許容誤差のために候補となる辺の集合があいまいになる場合も、列挙は一般の関数に切り替わる。

## 計算量

$N$ を多角形の辺数、$M$ を与える点の個数、$K$ を返す点と辺の接続の個数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `enumerate_points_on_polygon_boundary(...)` | $O(NM+K\log(K+1))$ | $O(M+K)$ |
| `PolygonBoundaryPointQuery(polygon)` | $O(N\log N)$（高速経路の検証・前処理） | $O(N)$ |
| `uses_convex_fast_path()` | $O(1)$ | $O(1)$ |
| `polygon()` | $O(1)$（内部の `std::vector` への `const&` を返す） | $O(1)$ |
| `enumerate(points, unique)`（凸の高速経路） | $O(M\log N+K\log(K+1))$ | $O(N+M+K)$ |
| `enumerate(points, unique)`（一般の経路） | $O(NM+K\log(K+1))$ | $O(M+K)$ |

コンストラクタは、最終的に一般の経路を選ぶ場合でも高速経路の検証を試みる。`polygon()` の返り値を全列挙するには別に $O(N)$ かかる。どちらの `enumerate` も長さ $K$ 以下の添字の列を返す。

## 注意点

座標と中間値は有限な `long double` の範囲に収まる必要がある。境界・退化の判定には 2 次元幾何で共通の許容誤差を用いる。

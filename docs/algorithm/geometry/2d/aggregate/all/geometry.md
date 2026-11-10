---
title: Geometry (二次元幾何)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/geometry.hpp
---

座標を `long double` で表す 2 次元幾何ライブラリの全 API をまとめて読み込む集約ヘッダである。特に断りがない限り、比較には絶対許容誤差 `GEOMETRY_EPS = 1e-10L` を用いる。`Point::operator<` は標準のソートアルゴリズムで使えるよう、誤差を考慮しない厳密な辞書順である。

以下に主な API を挙げる。このヘッダはほかにも多角形・円・三角形分割・Voronoi 図などの多数のヘッダを include しており、それらの詳細は各ヘッダのドキュメントを参照すること。

## 型

```cpp
Point
Line
Segment
Circle
```

`Segment` は `Line` の別名である。直線を扱う操作に渡す直線は、相異なる 2 点で定めなければならない。円の半径は非負でなければならない。

## 点とベクトル

```cpp
dot(a, b)
cross(a, b)
norm(p)
abs(p)
arg(p)
unit(p)
rotate90(p)
rotate(p, theta)
polar(r, theta)
ccw(a, b, c)
orthogonal(a, b)
parallel(a, b)
sort_points_by_argument(points)
```

`unit` は零ベクトルに対して `std::invalid_argument` を送出する。`ccw` は次のいずれかの値を返す。

```cpp
CLOCKWISE = -1
COUNTER_CLOCKWISE = 1
ONLINE_BACK = 2
ONLINE_FRONT = -2
ON_SEGMENT = 0
```

## 直線と線分

```cpp
projection(line, p)
reflection(line, p)
on_segment(segment, p)
intersect(s, t)
intersect(line, p)
same_line(a, b)
intersect_line_line(a, b)
intersect_line_segment(line, segment)
distance(a, b)
distance(line, p)
distance_segment_point(segment, p)
distance(s, t)
distance_line_line(a, b)
distance_line_segment(line, segment)
cross_point(a, b)
line_intersection(a, b)
segment_intersection(a, b)
```

`cross_point` と `line_intersection` は 2 直線のただ 1 つの交点を返す。平行または一致する直線に対しては `std::domain_error` を送出する。`segment_intersection` はただ 1 つの共有点を返し、交わらない場合や共通部分が線分になる場合は `std::domain_error` を送出する。退化した線分は点として扱う。直線を扱う操作に退化した直線を渡すと `std::invalid_argument` を送出する。

## 多角形

```cpp
area(polygon)
contains(polygon, p)
contains_convex(polygon, p)
ConvexPolygonQuery query(polygon)
query.contains(p)
is_convex(polygon)
convex_hull(points)
convex_diameter(convex_polygon)
convex_cut(polygon, line)
closest_pair(points)
```

`area` は符号付き面積を返す。`contains`、`contains_convex`、`ConvexPolygonQuery::contains` は、外部なら `0`、境界上なら `1`、内部なら `2` を返す。`ConvexPolygonQuery` は時計回り・反時計回りどちらの凸多角形も受け付け、連続する重複頂点と同一直線上の中間頂点を除き、全点が同一直線上なら両端の 2 点にまとめる。同じ多角形に何度も問い合わせる場合はこれを使う。`contains_convex` は 1 回の問い合わせごとに同じ正規化を行う。

`is_convex` と `convex_diameter` はどちらの向きも受け付ける。`convex_diameter` は同一直線上の頂点や連続する重複頂点を含んでもよい。`convex_hull` は同一直線上の中間点を含まない反時計回りの凸包を返す。`convex_cut` は有向直線の左側を残す。

## 円

```cpp
circle_relation(a, b)
intersect_circle_line(circle, line)
intersect_circle_segment(circle, segment)
intersect_circle_circle(a, b)
circle_circle_intersection_area(a, b)
circle_line_cross_points(circle, line)
circle_segment_cross_points(circle, segment)
circle_circle_cross_points(a, b)
tangent_points(circle, p)
tangent_lines(circle, p)
common_tangents(a, b)
circumcenter(a, b, c)
circumcircle(a, b, c)
minimum_enclosing_circle(points)
incenter(a, b, c)
incircle(a, b, c)
```

`circle_relation` は次の値を返す。

```cpp
CIRCLE_INSIDE = 0
CIRCLE_INTERNALLY_TANGENT = 1
CIRCLE_INTERSECT = 2
CIRCLE_EXTERNALLY_TANGENT = 3
CIRCLE_SEPARATE = 4
CIRCLE_COINCIDENT = 5
```

`circle_circle_intersection_area` は 2 つの円板の共通部分の面積を返す。離れている場合、外接する場合、または少なくとも一方の半径が $0$ の場合は $0$ を返し、包含・内接・同心・一致の場合は小さい方の円板の面積を返す。通常の 2 点で交わる場合は余弦を $[-1, 1]$ に収めて計算し、外接に近い小さな弓形には級数を用いる。どちらかの半径が負なら `std::invalid_argument` を送出する。

`circle_circle_cross_points` と `common_tangents` は、一致する円に対しては結果が無限個になるため `std::domain_error` を送出する。`circumcenter` と `incenter` は 3 点が同一直線上にある場合に `std::invalid_argument` を送出する。

## 計算量

$N$ を入力の多角形・点列の要素数、$H$ を返す凸包の頂点数とする。座標の四則演算・三角関数・平方根を $O(1)$ とみなす。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `geometry_sign(x)` / `GEOMETRY_EPS` 参照 | $O(1)$ | $O(1)$ |
| `Point` の各コンストラクタ、単項・二項・複合代入・比較の各演算子 | $O(1)$ | $O(1)$ |
| `operator*(k,p)`、`Point` / `Line` / `Circle` のフィールド参照・集成体初期化 | $O(1)$ | $O(1)$ |
| `dot` / `cross` / `norm` / `abs` / `arg` | $O(1)$ | $O(1)$ |
| `sort_points_by_argument(points)` | $O(N\log N)$ | $O(\log N)$ |
| `unit` / `rotate90` / `rotate` / `polar` / `ccw` | $O(1)$ | $O(1)$ |
| `orthogonal` / `parallel`（`Point`・`Line` のオーバーロード） | $O(1)$ | $O(1)$ |
| `projection` / `reflection` / `on_segment` | $O(1)$ | $O(1)$ |
| `intersect` の両オーバーロード / `same_line` / `intersect_line_line` / `intersect_line_segment` | $O(1)$ | $O(1)$ |
| `distance` の全オーバーロード / `distance_segment_point` / `distance_line_line` / `distance_line_segment` | $O(1)$ | $O(1)$ |
| `cross_point` / `line_intersection` / `segment_intersection` | $O(1)$ | $O(1)$ |
| `area(polygon)` / `contains(polygon, p)` / `is_convex(polygon)` | $O(N)$ | $O(1)$ |
| `ConvexPolygonQuery(polygon)` | $O(N)$（正規化前処理） | $O(N)$ |
| `ConvexPolygonQuery::size()` / `vertices()` | $O(1)$（メタデータ・参照の返却） | $O(1)$ |
| `ConvexPolygonQuery::contains(p)` | $O(\log N)$ | $O(1)$ |
| `contains_convex(polygon, p)` | $O(N)$ | $O(N)$ |
| `convex_hull(points)` | $O(N\log N)$ | 出力 $O(H)$、作業領域 $O(N)$ |
| `convex_diameter(convex_polygon)` | $O(N)$ | $O(N)$ |
| `convex_cut(polygon, line)` | $O(N)$ | 出力 $O(N)$ |
| `closest_pair(points)` | $O(N\log N)$ | $O(N)$ |
| `validate_circle(circle)` / `circle_relation(a, b)` | $O(1)$ | $O(1)$ |
| `circle_circle_intersection_area(a, b)` | $O(1)$ | $O(1)$ |
| `intersect_circle_line` / `intersect_circle_segment` / `intersect_circle_circle` | $O(1)$ | $O(1)$ |
| `circle_line_cross_points` / `circle_segment_cross_points` / `circle_circle_cross_points` | $O(1)$ | 最大2点 |
| `tangent_points` / `tangent_lines` | $O(1)$ | 最大2要素 |
| `common_tangents(a, b)` | $O(1)$ | 最大4本 |
| `circumcenter` / `circumcircle` / `incenter` / `incircle` | $O(1)$ | $O(1)$ |
| `minimum_enclosing_circle(points)` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |
| `MinimumEnclosingCircleResult` の各クエリ | $O(1)$ | $O(1)$ |

`vertices()` は内部の `std::vector` への `const&` を返すだけなので、全頂点を列挙するには別に $O(N)$ かかる。円に関する関数の `std::vector` の出力は要素数が定数で抑えられるので $O(1)$ に含めている。点・直線・円の個々の操作は $O(1)$ である。

## 注意点

座標と中間値は有限な `long double` の範囲に収まる必要がある。境界・退化の判定には、各 API に記した許容誤差を用いる。

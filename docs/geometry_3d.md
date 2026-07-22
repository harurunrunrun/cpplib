---
title: 3D Geometry (三次元幾何)
documentation_of: ../src/algorithm/geometry/3d/core/geometry_3d.hpp
---

`long double` を用いる3次元幾何の基本型、距離、交差、三角形、四面体、球をまとめる。判定には `GEOMETRY3D_EPS = 1e-10L` を使う。

## header

`geometry_3d.hpp` は後方互換用の umbrella header であり、下記の個別 header を
include するだけである。必要な API だけを使う場合は
`3d/<name>.hpp` を直接 include できる。

`base.hpp` は定数、`Point3` の member/operator と基本型だけを定義する。
公開関数は同名 overload 群ごとに次の header に分かれる。

| header | 公開 API |
| --- | --- |
| `geometry3d_sign.hpp` | `geometry3d_sign` |
| `dot.hpp` / `cross.hpp` | `dot` / `cross` |
| `norm.hpp` / `abs.hpp` / `unit.hpp` | `norm` / `abs` / `unit` |
| `scalar_triple.hpp` | `scalar_triple` |
| `parallel.hpp` / `orthogonal.hpp` | `parallel` / `orthogonal` |
| `collinear.hpp` / `coplanar.hpp` | `collinear` / `coplanar` |
| `line3_direction.hpp` / `ray3_direction.hpp` | 対応する direction |
| `plane3_unit_normal.hpp` | `plane3_unit_normal` |
| `on_line.hpp` / `on_ray.hpp` / `on_segment.hpp` / `on_plane.hpp` | 対応する `on_*` |
| `projection.hpp` | `projection` の2 overload |
| `closest_point.hpp` | `closest_point` の3 overload |
| `reflection.hpp` | `reflection` の2 overload |
| `distance.hpp` | `distance` の8 overload |
| `signed_distance.hpp` | `signed_distance` |
| `closest_points.hpp` | `closest_points` の2 overload |
| `line_plane_intersection.hpp` | `line_plane_intersection` |
| `ray_plane_intersection.hpp` | `ray_plane_intersection` |
| `segment_plane_intersection.hpp` | `segment_plane_intersection` |
| `plane_plane_intersection.hpp` | `plane_plane_intersection` |
| `line_sphere_intersections.hpp` | `line_sphere_intersections` |
| `ray_sphere_intersections.hpp` | `ray_sphere_intersections` |
| `segment_sphere_intersections.hpp` | `segment_sphere_intersections` |
| `plane_sphere_intersection.hpp` | `plane_sphere_intersection` |
| `sphere_sphere_intersection.hpp` | `sphere_sphere_intersection` |
| `triangle_normal.hpp` / `area.hpp` | `triangle_normal` / `area` |
| `centroid.hpp` | `centroid` の2 overload |
| `barycentric_coordinates.hpp` | `barycentric_coordinates` の2 overload |
| `contains.hpp` | `contains` の3 overload |
| `signed_volume.hpp` / `volume.hpp` | `signed_volume` / `volume` |
| `circumcircle.hpp` / `circumsphere.hpp` | `circumcircle` / `circumsphere` |
| `on_circle.hpp` | `on_circle` |
| `sphere_surface_area.hpp` / `sphere_volume.hpp` | 球の表面積 / 体積 |
| `sphere_cap_surface_area.hpp` / `sphere_cap_volume.hpp` | 球冠の曲面積 / 体積 |
| `sphere_intersection_volume.hpp` | `sphere_intersection_volume` |
| `angle.hpp` / `rotate_around_axis.hpp` | `angle` / `rotate_around_axis` |
| `plane_orthonormal_basis.hpp` | `plane_orthonormal_basis` |

全公開定義は `inline` であり、個別 header と umbrella header を同じ翻訳単位で
重複して include しても定義は一度だけ読み込まれる。


## 基本型

~~~cpp
Point3{x, y, z}
Line3{a, b}
Ray3{origin, through}
Segment3{a, b}
Plane3{point, normal}
Circle3{center, normal, radius}
Sphere3{center, radius}
Triangle3{a, b, c}
Tetrahedron3{a, b, c, d}
~~~

`Line3` は `a`, `b` を通る直線、`Ray3` は `origin` から `through` 方向への半直線、`Plane3` は `point` を通り `normal` を法線とする平面である。`Circle3` は `normal` に垂直な平面上の円を表す。

`Point3` は単項符号、加減算、scalar倍・除算、複合代入、誤差付き等値比較、辞書順比較を持つ。各演算は `O(1)` 時間計算量・空間計算量。

## ベクトル

| API | 説明 |
| --- | --- |
| `geometry3d_sign(value)` | EPS付き符号を `-1,0,1` で返す。 |
| `dot(a,b)` | 内積。 |
| `cross(a,b)` | 外積。 |
| `norm(p)` | 長さの二乗。 |
| `abs(p)` | Euclidean length。 |
| `unit(p)` | 単位ベクトル。零ベクトルでは `invalid_argument`。 |
| `scalar_triple(a,b,c)` | scalar triple product。 |
| `parallel(a,b)` | 2ベクトルが平行か判定する。 |
| `orthogonal(a,b)` | 2ベクトルが直交するか判定する。 |
| `collinear(a,b,c)` | 3点が同一直線上か判定する。 |
| `coplanar(a,b,c,d)` | 4点が同一平面上か判定する。 |
| `angle(a,b)` | 2ベクトル間の角を `[0,pi]` で返す。零ベクトルでは例外。 |
| `rotate_around_axis(p,axis,theta)` | 原点を通る `axis` の周りにRodriguesの公式で回転する。 |

すべて `O(1)` 時間計算量・空間計算量。

## 所属、射影、鏡映

| API | 説明 |
| --- | --- |
| `line3_direction(line)` | 非零の方向ベクトルを返す。退化直線では例外。 |
| `ray3_direction(ray)` | 非零の方向ベクトルを返す。退化半直線では例外。 |
| `plane3_unit_normal(plane)` | 単位法線を返す。零法線では例外。 |
| `on_line(line,p)` | 点が直線上か判定する。 |
| `on_ray(ray,p)` | 点が半直線上か判定する。originを含む。 |
| `on_segment(segment,p)` | 点が閉線分上か判定する。退化線分にも対応する。 |
| `on_plane(plane,p)` | 点が平面上か判定する。 |
| `projection(line,p)` | 点の直線への直交射影。 |
| `projection(plane,p)` | 点の平面への直交射影。 |
| `closest_point(ray,p)` | 半直線上の最近点。 |
| `closest_point(segment,p)` | 閉線分上の最近点。退化線分にも対応する。 |
| `reflection(line,p)` | 点を直線に関して180度回転した点。 |
| `reflection(plane,p)` | 点の平面に関する鏡映。 |
| `plane_orthonormal_basis(plane)` | 平面上の右手系正規直交基底2本を返す。 |

すべて `O(1)` 時間計算量・空間計算量。

## 距離と最近点対

~~~cpp
distance(Point3, Point3)
distance(Line3, Point3)
distance(Ray3, Point3)
distance(Segment3, Point3)
signed_distance(Plane3, Point3)
distance(Plane3, Point3)
closest_points(Line3, Line3)
distance(Line3, Line3)
closest_points(Segment3, Segment3)
distance(Segment3, Segment3)
distance(Triangle3, Point3)
~~~

`closest_points` は2つの図形上で距離を最小にする点のpairを返す。平行直線では一意でない最近点対のうち1組を返す。退化線分にも対応する。各APIは `O(1)` 時間計算量・空間計算量。

## 平面との交差

| API | 戻り値・条件 |
| --- | --- |
| `line_plane_intersection(line,plane)` | 一意な交点。平行または直線全体が平面上なら `domain_error`。 |
| `ray_plane_intersection(ray,plane)` | 半直線上の一意な交点。存在しない、または無数にある場合は `nullopt`。 |
| `segment_plane_intersection(segment,plane)` | 閉線分上の一意な交点。存在しない、または非退化線分全体が平面上なら `nullopt`。平面上の退化線分はその点を返す。 |
| `plane_plane_intersection(a,b)` | 一意な交線。平行・一致平面では `domain_error`。 |

すべて `O(1)` 時間計算量・空間計算量。

## 球との交差

| API | 説明 | 戻り値の最大要素数 |
| --- | --- | --- |
| `line_sphere_intersections(line,sphere)` | 直線と球面の交点。直線の向きに沿って並ぶ。 | 2 |
| `ray_sphere_intersections(ray,sphere)` | 半直線と球面の交点。 | 2 |
| `segment_sphere_intersections(segment,sphere)` | 閉線分と球面の交点。退化線分にも対応する。 | 2 |
| `plane_sphere_intersection(plane,sphere)` | 交円。交わらなければ `nullopt`、接する場合は半径0。 | 1 |
| `sphere_sphere_intersection(a,b)` | 交円。交わらなければ `nullopt`、接する場合は半径0。一致球では交円が一意でないため `domain_error`。 | 1 |

各APIは返す `vector` の確保を含めて `O(1)` 時間計算量・空間計算量。負の半径では `invalid_argument`。

## 三角形

| API | 説明 |
| --- | --- |
| `triangle_normal(triangle)` | 単位法線。退化三角形では例外。 |
| `area(triangle)` | 面積。 |
| `centroid(triangle)` | 重心。 |
| `barycentric_coordinates(triangle,p)` | 重心座標 `{wa,wb,wc}`。点は平面外でもよい。退化時は例外。 |
| `contains(triangle,p)` | 点が閉三角形上にあるか判定する。 |
| `closest_point(triangle,p)` | 閉三角形上の最近点。退化時は例外。 |
| `distance(triangle,p)` | 点と閉三角形の距離。 |
| `circumcircle(triangle)` | 3頂点を通る平面上の外接円。退化時は例外。 |

すべて `O(1)` 時間計算量・空間計算量。

## 四面体

| API | 説明 |
| --- | --- |
| `signed_volume(tetrahedron)` | 向き付き体積。 |
| `volume(tetrahedron)` | 非負の体積。 |
| `centroid(tetrahedron)` | 重心。 |
| `barycentric_coordinates(tetrahedron,p)` | 重心座標 `{wa,wb,wc,wd}`。退化時は例外。 |
| `contains(tetrahedron,p)` | 点が閉四面体内にあるか判定する。退化時は例外。 |
| `circumsphere(tetrahedron)` | 4頂点を通る外接球。退化時は例外。 |

すべて `O(1)` 時間計算量・空間計算量。

## 球と円

| API | 説明 |
| --- | --- |
| `contains(sphere,p)` | 内部なら1、球面上なら0、外部なら-1。 |
| `on_circle(circle,p)` | 点が3次元円周上か判定する。 |
| `sphere_surface_area(sphere)` | 球の表面積。 |
| `sphere_volume(sphere)` | 球の体積。 |
| `sphere_cap_surface_area(sphere,height)` | 高さ `height` の球冠の曲面積。 |
| `sphere_cap_volume(sphere,height)` | 高さ `height` の球冠の体積。 |
| `sphere_intersection_volume(a,b)` | 2つの閉球の共通部分体積。離隔・内包・一致にも対応する。 |

すべて `O(1)` 時間計算量・空間計算量。半径は非負、球冠の高さは `[0,2*radius]` でなければならず、違反時は `invalid_argument`。

## 注意点

- 入力と中間値が有限な `long double` の範囲に収まる必要がある。
- EPS判定は絶対誤差である。巨大座標や厳密な整数述語が必要な問題では、整数専用の `__int128` 判定を使う。
- `Line3`, `Ray3`, `Plane3` を使うAPIは非退化な方向・法線を要求する。退化時は `invalid_argument`。
- 交点の存在と一意性を区別するAPIでは、無数の交点を1点へ縮約しない。

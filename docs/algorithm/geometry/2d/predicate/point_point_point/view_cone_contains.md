---
title: View Cone Contains (視野角の包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_point_point/view_cone_contains.hpp
---

観測点から方向ベクトルを中心に左右対称に広がる視野角に、点が入るかを判定する。

## API

~~~cpp
bool view_cone_contains(
    const Point& observer,
    const Point& direction,
    long double aperture,
    const Point& point,
    bool include_boundary = true
)
~~~

`aperture` は視野全体の角度（ラジアン）であり、中心方向から左右へ $\mathrm{aperture} / 2$ ずつ広がる。`include_boundary` が `true` なら両側の境界の半直線を含み、`false` なら除く。観測点自身（距離が `GEOMETRY_EPS` 以下の点）は常に含む。

## 時間計算量

$O(1)$。

## 空間計算量

$O(1)$。

## 注意点

- `direction` は長さが `GEOMETRY_EPS` より大きいベクトル、`aperture` は $0$ 以上 $2\pi$ 以下（許容誤差 `GEOMETRY_EPS`）でなければならない。違反時は `std::invalid_argument` を送出する。
- `Point` の各座標と `aperture` は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- `aperture` が $0$ のときは、境界を含む場合に限り中心方向の前向きの半直線を含む。`aperture` が $2\pi$ のときは `include_boundary` によらず全方向を含む。
- 境界の比較では、角度に対して絶対許容誤差 `GEOMETRY_EPS` を用いる。

---
title: View Cone Contains (視野角包含判定)
documentation_of: ../src/algorithm/geometry/2d/view_cone_contains.hpp
---

観測点から方向vectorを中心に広がる対称な視野角に、点が入るかを判定する。

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

apertureは視野全体の角度であり、中心方向から左右へaperture / 2ずつ広がる。include_boundaryがtrueなら両境界rayを含み、falseなら除く。観測点自身は常に含む。

## 時間計算量

O(1)。

## 空間計算量

O(1)。

## 注意点

- directionは非零vector、apertureは0以上2π以下であること。違反時はinvalid_argument。
- Pointの各座標とapertureは有限値であること。違反時はinvalid_argument。
- aperture 0は、境界を含む場合だけ中心の前向きrayを含む。aperture 2πは全方向を含む。
- 境界比較にはGEOMETRY_EPSを用いる。

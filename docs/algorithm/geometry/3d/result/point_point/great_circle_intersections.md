---
title: Great-Circle Intersections (大円どうしの交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_point/great_circle_intersections.hpp
---

単位球面上の 2 つの大円の交点を求める。

## API

- `great_circle_intersections(first_normal, second_normal)`: 2 つの大円をそれぞれ含む、原点を通る平面の法線を受け取り、単位球面上で互いに対蹠な 2 交点を `std::optional<std::pair<Point3, Point3>>` で返す。2 つの大円が同じなら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。

## 注意点

- 法線は有限な非零ベクトルでなければならない。違反した場合は `std::invalid_argument` を送出する。
- 平行・反平行な法線は同じ大円を表す。正規化した法線の外積のノルムが $64 \varepsilon$（$\varepsilon$ は `long double` の機械イプシロン）以下なら同じ大円とみなすため、非常に近い大円も同一と判定されることがある。

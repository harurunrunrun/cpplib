---
title: Intersection of Three 3D Planes (三次元3平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/three_plane_intersection.hpp
---

3 平面の共通部分を表す結果型と、それを計算する関数をまとめて読み込む互換用の集約ヘッダである。結果型と計算関数は、それぞれの個別ヘッダからも利用できる。

## API

- `ThreePlaneIntersection3`: 空集合（`std::monostate`）・1 点（`Point3`）・直線（`Line3`）・平面（`Plane3`）のいずれかを表す `std::variant`。
- `three_plane_intersection(first, second, third)`: 3 平面すべての共通部分を返す。

## API別の時間計算量・空間計算量

型の操作と計算関数は、いずれも時間・追加領域ともに $O(1)$ である。

## 注意点

入力平面は有限で、法線は非零でなければならない。違反時は `std::invalid_argument` を送出する。平行・一致の判定は許容誤差付きで行う。

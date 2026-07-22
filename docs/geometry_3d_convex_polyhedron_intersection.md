---
title: Convex Polyhedron Intersection (凸多面体同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/shape/convex_polyhedron_intersection.hpp
---

外向き三角形面で表された二つの凸多面体の共通部分を
`ConvexPolyhedron3` として返す。

## API

```cpp
ConvexPolyhedron3 convex_polyhedron_intersection(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
)

ConvexPolyhedron3 convex_polyhedron_intersection_via_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
)

ConvexPolyhedron3
convex_polyhedron_intersection_via_halfspaces_with_interior_point(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const Point3& strict_interior_point
)
```

- `convex_polyhedron_intersection` は、相手に含まれる入力頂点と、
  相手でclipした全入力辺の端点を列挙し、その凸包を返す。空集合や
  lower-dimensionalな接触も追加の前提なしで扱う汎用版である。
- `convex_polyhedron_intersection_via_halfspaces` は、両入力の外向き面を
  $normal\cdot(x-point)\leq0$ の半空間へ変換し、極双対凸包から共通部分を構築する。
  正体積の共通部分では高速経路を使う。空集合なら空の多面体を返し、
  lower-dimensionalな接触も返すが、この二種類は退化処理を使う。
- `convex_polyhedron_intersection_via_halfspaces_with_interior_point` は、
  共通部分の狭義内点を呼出側が与える高速版である。

## API別の時間計算量・空間計算量

$V_i,E_i,F_i$ を各入力の頂点数・辺数・三角形面数、
$C$ を従来版が列挙する候補点数、$H=F_1+F_2$、
$K$ を返す頂点と三角形面の総数とする。exact整数演算は1回と数える。

- `convex_polyhedron_intersection`:
  時間 $O(V_1F_2+V_2F_1+E_1F_2+E_2F_1)$ に加え、
  候補点凸包が入力と独立な乱択順なら期待 $O(C\log C+K)$。
  固定seedに対する決定的最悪時間は $O(C^2\log C+K)$。
  追加領域は期待 $O(C+K)$、最悪 $O(C^2+K)$。
- `convex_polyhedron_intersection_via_halfspaces`:
  共通部分がfull-dimensionalかつ非空なら、内点探索を含めて期待
  $O(H\log H+K)$、期待領域 $O(H+K)$。空集合、lower-dimensional、
  または中間射影を表現できない場合の退化経路は最悪
  $O(H^4+K)$、領域 $O(H+K)$。
- `convex_polyhedron_intersection_via_halfspaces_with_interior_point`:
  期待時間 $O(H\log H+K)$、期待領域 $O(H+K)$。
  固定seedに対する決定的最悪時間は $O(H^2\log H+K)$、
  最悪領域は $O(H^2+K)$。

## 注意点

- 半空間版の両入力は `affine_dimension == 3` で、非空の
  `vertices` と外向き・非退化な `faces` を持たなければならない。
  違反時は `invalid_argument`。
- 面の頂点添字が範囲外なら `invalid_argument`。
- 内点付き版の `strict_interior_point` は全ての入力面を狭義に
  満たす有限点でなければならず、違反時は `invalid_argument`。
- 半空間版で非空かつ非有界と判定された場合は `domain_error`。
  正しい有界凸多面体を入力すれば、共通部分も有界である。
- 同一平面を分割した複数の三角形面は重複制約として安全に扱われる。

---
title: Convex Polyhedron Intersects (凸多面体同士の交差判定)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_intersects.hpp
---

## API

```cpp
bool convex_polyhedron_intersects(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
);
```

Minkowski差に対するGJK法で、境界接触を含め共通部分が空でないか返す。
共通部分の頂点・面は構築しない。2引数の既存呼出しもそのまま使える。

## API別の時間計算量・空間計算量

頂点数を $V_1,V_2$、実行した反復数を $I\leq max\_iterations$ とする。
各support queryは全頂点を1回走査するため、時間は
$O(I(V_1+V_2))$、正規化した入力を含む追加領域は $O(V_1+V_2)$ である。
既定の反復上限96を定数と見れば時間は $O(V_1+V_2)$ となる。

## 注意点

- 頂点列は非空かつ有限で、各入力の凸包を表さなければならない。面列は参照しない。
- `tolerance` は正の有限値、`max_iterations` は正でなければならない。
- 入力を共通平行移動とscaleで正規化してから判定するため、`tolerance` は
  相対的な接触許容誤差である。
- 反復上限へ達した場合は、その時点の最近simplexと許容誤差から判定する。
- 実際の共通部分が必要な場合は `convex_polyhedron_intersection` を使う。

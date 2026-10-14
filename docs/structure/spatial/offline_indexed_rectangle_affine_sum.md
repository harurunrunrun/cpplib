---
title: Offline Indexed Rectangle Affine Sum (点 ID 付き長方形アフィン作用と和) [dynamic_point_set_rectangle_affine_rectangle_sum]
documentation_of: ../../../src/structure/spatial/offline_indexed_rectangle_affine_sum.hpp
---

`OfflineIndexedRectangleAffineSum<T>(coordinates)` に全点の座標を先に登録する。
登録順の点 ID を使い、各点は最初は無効である。同じ座標の点も独立に管理する。

- `point_set(id,value)`: 点を有効化するか、現在値を置き換える。
- `point_erase(id)`: 点を無効化し、以前有効なら `true`。再び `point_set` すると有効化する。
- `rectangle_affine(l,d,r,u,a,b)`: 現在有効な半開長方形内の点へ `value -> a*value+b` を適用。
- `rectangle_sum(l,d,r,u)`: 現在有効な点の値の総和。
- `size()`: 登録点数。

## 時間計算量

登録点数 $N$ として構築は `nth_element` の平均線形性のもとで $O(N\log N)$、
点更新 $O(\log N)$、長方形更新・照会 $O(\sqrt N)$、保存空間 $O(N)$。

## 注意点

無効だった期間に行った作用は、その後の有効化でその点へ適用しない。
座標は負を含む `int64_t`。長方形は `l<=r`, `d<=u`、逆順なら `invalid_argument`。
`T` は整数からの構築、比較、加減乗算を提供し、中間結果を表現できる必要がある。

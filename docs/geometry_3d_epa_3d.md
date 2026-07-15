---
title: Expanding Polytope Algorithm 3D (3次元拡張ポリトープ法)
documentation_of: ../src/algorithm/geometry/3d/epa_3d.hpp
---

## API

- `epa_3d(first, second, gjk_result, tolerance=1e-10L, max_iterations=128)`: GJK 終了単体から Minkowski 差境界を拡張し、貫通深度・接触法線・接触点を返す。
- `epa_3d(first, second, tolerance=1e-10L, max_iterations=128)`: 内部で GJK を実行する簡便版。

## API別の時間計算量・空間計算量

- support 1回は $O(V_1+V_2)$。EPA が $I$ 回反復し最大 $F$ 面を持つとき時間 $O(I(V_1+V_2+F))$、領域 $O(I+F)$。退化初期単体の構築は定数個の support と高々定数個の4点組を調べる。

## 注意点

GJK が非交差なら `intersects=false`。面・辺接触など正の体積を持つ初期単体を作れない場合は、同じ候補軸を備えた SAT で深さと法線を確定する。非正の許容誤差や反復上限0は `std::invalid_argument`。

---
title: Expanding Polytope Algorithm 3D (3次元拡張ポリトープ法)
documentation_of: ../src/algorithm/geometry/3d/query/epa_3d.hpp
---

## API

- `epa_3d(first, second, gjk_result, tolerance=1e-10L, max_iterations=128)`: GJK 終了単体から Minkowski 差境界を拡張し、貫通深度・接触法線・接触点を返す。
- `epa_3d(first, second, tolerance=1e-10L, max_iterations=128)`: 内部で GJK を実行する簡便版。

## API別の時間計算量・空間計算量

- `epa_3d(first, second, gjk_result, ...)`: support 1回は $O(V_1+V_2)$。EPA が $I_E$ 回反復し最大 $F$ 面を持つとき時間 $O(I_E(V_1+V_2+F))$、領域 $O(I_E+F)$。退化初期単体の構築は定数個の support と高々定数個の4点組を調べる。
- `epa_3d(first, second, ...)`: 上記にGJKの $I_G$ 反復を加え、時間 $O(I_G(V_1+V_2)+I_E(V_1+V_2+F))$、領域 $O(I_E+F)$。
- 初期ポリトープを作れずSATへfallbackする場合は、SATの面数・辺数依存の計算量を別途要する。

## 注意点

GJK が非交差なら `intersects=false`。面・辺接触など正の体積を持つ初期単体を作れない場合は、同じ候補軸を備えた SAT で深さと法線を確定する。非正の許容誤差や反復上限0は `std::invalid_argument`。

EPAの貫通深度は3次元内部を持つ凸多面体同士に対して定義する。どちらかの `affine_dimension != 3` なら、両overloadとも `std::invalid_argument`。

両 overload とも共通平行移動を除去し全頂点の広がりで無次元化した同一coreを用いる。`gjk_result` 付きoverloadは、support点を同じ変換で正規化してから利用する。単体が空なら正規化座標上でGJKを補完実行し、非有限のsupport点は `std::invalid_argument`。

`tolerance` は形状の広がりに対する相対許容誤差。反復上限到達時は現在の最良面を返して `converged=false` とする。

貫通深度と接触点は入力座標系へ戻す。真値が `long double` で表現不能なら `std::overflow_error`。`normal` は交差時に有限な単位ベクトルである。

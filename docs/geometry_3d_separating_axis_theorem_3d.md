---
title: Separating Axis Theorem 3D (3次元分離軸定理)
documentation_of: ../src/algorithm/geometry/3d/separating_axis_theorem_3d.hpp
---

## API

- `separating_axis_theorem_3d(first, second, tolerance=1e-12L)`: 両形状の面法線と全ての辺方向ペアの外積を検査し、分離・重なり結果を返す。

## API別の時間計算量・空間計算量

- 面数を $F_i$、辺数を $E_i$、頂点数を $V_i$、重複除去後の軸数を $A$ として、時間 $O((F_1+F_2+E_1E_2)A+A(V_1+V_2))$、領域 $O(A+E_1+E_2)$。

## 注意点

三角形面が外向きである必要はないが、両入力は面と辺を持つ3次元凸多面体でなければならない。空・低次元入力、非正の許容誤差、不正な面添字を例外で拒否する。

全候補軸の生成と射影は、共通平行移動を除去し全頂点の広がりで無次元化した座標で行う。`tolerance` はその相対許容誤差であり、一様scaleと共通平行移動に依存しない。

`separation` と `penetration_depth` は入力座標系へ戻す。真値が `long double` で表現不能なら `std::overflow_error`。`axis` は結果が定義される場合に有限な単位ベクトルである。

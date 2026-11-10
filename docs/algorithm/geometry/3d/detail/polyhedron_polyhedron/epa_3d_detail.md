---
title: EPA 3D Internal Core (三次元EPA内部コア)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron_polyhedron/epa_3d_detail.hpp
---

## API

- `epa_3d_detail::validate_full_dimensional(first, second)`: 両方の凸多面体が有限で、三次元の内部を持つ（アフィン次元が 3）ことを検査する。違反時は `std::invalid_argument` を送出する。
- `epa_3d_detail::epa_core(first, second, gjk_result, tolerance, max_iterations)`: 正規化済みの形状と GJK の結果から EPA を実行する内部関数。`tolerance` は有限な正の値でなければならない。
- `epa_3d_detail::restore_result(result, normalization)`: EPA の結果を入力の座標系へ戻す内部関数。

## API別の時間計算量・空間計算量

- `validate_full_dimensional`: 頂点数を $V_1, V_2$ として時間 $O(V_1+V_2)$、追加領域 $O(1)$。
- `epa_core`: 支持点の計算 1 回が $O(V_1+V_2)$。反復回数を $I_E$、最大面数を $F$ として、時間 $O(I_E(V_1+V_2+F))$、領域 $O(I_E+F)$。
- `restore_result`: 時間・追加領域ともに $O(1)$。

## 注意点

公開オーバーロードから共有される内部実装であり、直接利用するための安定した API ではない。入力は GJK の正規化座標系で与える必要がある。

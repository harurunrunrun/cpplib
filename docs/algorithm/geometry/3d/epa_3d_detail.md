---
title: EPA 3D Internal Core (3次元EPA内部コア)
documentation_of: ../../../../src/algorithm/geometry/3d/epa_3d_detail.hpp
---

## API

- `epa_3d_detail::validate_full_dimensional(first, second)`: 両凸多面体が有限かつ三次元内部を持つことを検査する。
- `epa_3d_detail::epa_core(first, second, gjk_result, tolerance, max_iterations)`: 正規化済み形状とGJK結果からEPAを実行する内部関数。
- `epa_3d_detail::restore_result(result, normalization)`: EPA結果を入力座標系へ戻す内部関数。

## API別の時間計算量・空間計算量

- `validate_full_dimensional`: 頂点数を $V_1,V_2$ として時間 $O(V_1+V_2)$、追加領域 $O(1)$。
- `epa_core`: support 1回が $O(V_1+V_2)$、反復回数を $I_E$、最大面数を $F$ として時間 $O(I_E(V_1+V_2+F))$、領域 $O(I_E+F)$。
- `restore_result`: 時間・追加領域 $O(1)$。

## 注意点

公開overloadから共有される内部実装であり、直接利用するための安定APIではない。入力はGJK正規化座標系である必要がある。

---
title: Real Gaussian Elimination Result (実数ガウス消去結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/real_gaussian_elimination_result.hpp
---

実数行列の行基本変形結果とpivot列を保持する型。

## API

- `reduced_row_echelon_form`: 簡約行階段形。
- `pivot_columns`: pivotを持つ列添字を行順に格納した列。
- `rank()`: pivot数、すなわち数値rankを返す。

## API別の時間計算量・空間計算量

- 各フィールドへの参照と `rank()`: 時間・追加領域ともに $O(1)$。
- $R\times C$ の行列とrank $K$ を保持する領域: $O(RC+K)$。

## 注意点

この型自身は行列の矩形性、pivot列の順序、rankとの整合性を検査しない。

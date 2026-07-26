---
title: Real Linear Algebra Internal Detail (実数線形代数内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/gaussian_elimination_real_detail.hpp
---

実数行列検証、許容誤差検証、scale計算を二つの公開leafで共有する。

## API

- `validate_real_matrix(matrix)`: 矩形性・有限性を検査し、行数と列数を返す内部関数。
- `validate_relative_epsilon(epsilon)`: 許容誤差を検査する内部関数。
- `maximum_absolute_entry(matrix)`: 最大絶対要素を返す内部関数。

## API別の時間計算量・空間計算量

$R\times C$ 行列について `validate_real_matrix` と `maximum_absolute_entry` は時間 $O(RC)$、追加領域 $O(1)$。`validate_relative_epsilon` は $O(1)$。

## 注意点

内部実装用headerであり互換性を保証しない。公開APIにはRREFまたは正方連立方程式solverのleafを使う。

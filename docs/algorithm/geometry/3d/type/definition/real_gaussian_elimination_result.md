---
title: Real Gaussian Elimination Result (実数ガウス消去結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/real_gaussian_elimination_result.hpp
---

実数行列に行基本変形を施した結果と、ピボット列を保持する型。

## API

- `reduced_row_echelon_form`: 簡約行階段形（`std::vector<std::vector<long double>>`）。
- `pivot_columns`: ピボットを持つ列の添字を、行の順に格納した列（`std::vector<std::size_t>`）。
- `rank()`: ピボットの個数、すなわち数値的な階数を返す。

## API別の時間計算量・空間計算量

- 各メンバの参照と `rank()`: 時間・追加領域ともに $O(1)$。
- $R \times C$ の行列と階数 $K$ を保持する領域: $O(RC + K)$。

## 注意点

この型自体は、行列が矩形であるか、ピボット列の順序、階数との整合性を検査しない。

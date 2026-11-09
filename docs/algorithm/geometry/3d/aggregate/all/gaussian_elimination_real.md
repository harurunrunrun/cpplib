---
title: Real Gaussian Elimination (実数ガウス消去法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/gaussian_elimination_real.hpp
---

このヘッダは既約行階段形（RREF）の構築と、正方な連立一次方程式のソルバの個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な戻り値型の個別ヘッダだけを include すればよい。

`long double` の行列に対し、絶対値最大の要素をピボットに選ぶ部分ピボット選択付きのガウスの消去法を行う。入力全体を最大絶対値で正規化してから計算するため、座標のスケールを一様に変えてもランクの判定は変わらない。

## API

```cpp
RealGaussianEliminationResult gaussian_elimination_real(
    const std::vector<std::vector<long double>>& matrix,
    long double relative_epsilon = GEOMETRY3D_EPS
);
std::optional<std::vector<long double>> solve_square_linear_system_real(
    const std::vector<std::vector<long double>>& matrix,
    const std::vector<long double>& rhs,
    long double relative_epsilon = GEOMETRY3D_EPS
);
```

- `gaussian_elimination_real(matrix, relative_epsilon)`: 既約行階段形とピボット列を持つ `RealGaussianEliminationResult` を返す。
- `solve_square_linear_system_real(matrix, rhs, relative_epsilon)`: 正方行列 $A$ について $Ax = b$ の一意解を返す。数値的に特異なら `std::nullopt` を返す。

## 時間計算量

- $R \times C$ 行列の消去: 時間 $O(RC \min(R, C))$、追加空間 $O(RC)$。
- $N$ 元の求解: 時間 $O(N^3)$、追加空間 $O(N^2)$。

## 注意点

- 行の長さが揃っていない入力、非有限値を含む入力、許容誤差が不正な入力には `std::invalid_argument` を送出する。
- 正規化後の絶対値が `relative_epsilon` 以下のピボットは 0 とみなす。デフォルト値は `GEOMETRY3D_EPS` である。
- 返す既約行階段形は入力全体を正の定数で割った行列のものだが、既約行階段形はスカラー倍で変わらないため、元の行列のものと等しい。
- 解を `long double` で表現できない場合は `std::overflow_error` を送出する。

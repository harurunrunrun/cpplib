---
title: Real Matrix Rank (実数行列の階数)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index/scalar/matrix_rank_real.hpp
---

`long double` の一般の矩形行列の数値的な階数を求める。

## API

- `matrix_rank_real(matrix, relative_epsilon = GEOMETRY3D_EPS)`: 部分ピボット選択付きの Gauss の消去法で得たピボットの個数を返す。

## 時間計算量

$R\times C$ 行列に対し、時間 $O(RC\min(R,C))$、追加領域 $O(RC)$。

## 注意点

- 入力全体の最大絶対値で正規化した後、絶対値が `relative_epsilon` 以下のピボットを 0 とみなす。
- 行の長さが揃っていない場合、非有限な値がある場合、許容誤差が負または非有限の場合は `std::invalid_argument` を送出する。

---
title: Least Squares by QR (QR分解による最小二乗法)
documentation_of: ../src/algorithm/geometry/3d/query/least_squares.hpp
---

過決定または正方な実数線形系について、$\lVert Ax-b\rVert_2$ を最小化する
一意解をmodified Gram–Schmidt QR分解で求める。各列を正規化し、直交化を
2回行って丸め誤差を抑える。

## API

- `least_squares(matrix, rhs, relative_epsilon)`: full column rankなら最小二乗解を
  返す。列rankが不足する、または行数が列数未満なら `std::nullopt`。

## 時間計算量

$R\times C$ 行列に対し $O(RC^2)$ 時間、$O(RC+C^2)$ 追加領域。

## 注意点

- rank deficientな場合の解集合やMoore–Penrose疑似逆行列は返さない。
- 行長や右辺sizeの不一致、非有限値、不正な許容誤差には
  `std::invalid_argument` を送出する。
- 表現不能な列normまたは解には `std::overflow_error` を送出する。

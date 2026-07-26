---
title: 3D Principal Axes (三次元主成分軸)
documentation_of: ../../../../src/algorithm/geometry/3d/principal_axes_3d.hpp
---

点集合の重心と母共分散行列を計算し、分散の降順に3本のprincipal axisを返す。

## API

- `principal_axes_3d(points, relative_epsilon)`: 点集合の母共分散行列を分解し、
  重心・主成分軸・分散を `PrincipalAxes3` で返す。

## 時間計算量

点数を $N$ とすると $O(N)$ 時間、戻り値を除く追加領域 $O(1)$。

## 注意点

- 共分散は $1/N$ で割る母分散として定義する。
- 空集合、非有限点、不正な許容誤差には `std::invalid_argument` を送出する。
- 分散が `long double` に表現不能なら `std::overflow_error` を送出する。
- 重複した分散に対応する軸は一意でない。

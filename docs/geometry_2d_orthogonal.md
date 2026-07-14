---
title: 2D Orthogonality (二次元直交判定)
documentation_of: ../src/algorithm/geometry/2d/orthogonal.hpp
---

## API

- `orthogonal(left, right)`: 2ベクトルが直交するか判定する。
- `orthogonal(left_line, right_line)`: 2直線の方向ベクトルが直交するか判定する。

## API別の時間計算量・空間計算量

- 各overload: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

2ベクトルの長さの積をscaleとする許容誤差付き内積判定を行うため、各ベクトルの非零定数倍に対して判定は不変である。
零ベクトルを含むベクトルoverloadは従来互換のため `true` を返す。
直線overloadは退化直線に `std::invalid_argument` を送出する。

---
title: Fast Polynomial Interpolation (高速多項式補間)
documentation_of: ../src/algorithm/math/polynomial_interpolation_fast.hpp
---

積木と高速多項式積を用いて、任意点の値から係数列を復元する。

## API

- `math::polynomial_interpolation<MOD>(points, values)`: `f(points[i]) == values[i]` を満たす次数 `points.size()-1` 以下の多項式を、定数項から昇冪順で返す。末尾の0は除く。

## API別の時間計算量・空間計算量

- 標本数を $N$、次数 $N$ の積の計算量を $M(N)$ として、時間 $O(M(N)\log N)$、追加領域 $O(N\log N)$。
- この実装のNTT可能なmodulusでは $M(N)=O(N\log N)$。

## 注意点

2列の長さが異なる場合、または `Modint<MOD>` 上で評価点が重複する場合は `std::invalid_argument` を送出する。NTT長が `MOD-1` を割り切る必要がある。空入力は空の係数列を返す。

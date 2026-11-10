---
title: Fast Polynomial Interpolation (高速多項式補間)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_interpolation_fast.hpp
---

積の木（product tree）と高速な多項式の積を用いて、任意の点での値から係数の列を復元する。

## API

- `math::polynomial_interpolation<MOD>(points, values)`: $f(p_i) = v_i$（$p_i$ は `points[i]`、$v_i$ は `values[i]`）を満たす次数 $\lvert \mathrm{points} \rvert - 1$ 以下の多項式 $f$ を、定数項から次数の昇順の係数の列で返す。末尾の $0$ は取り除く。

## 時間計算量・空間計算量

- 標本の個数を $N$、次数 $N$ の多項式の積の計算量を $M(N)$ として、時間 $O(M(N) \log N)$、追加領域 $O(N \log N)$。
- この実装の NTT が使える法では $M(N) = O(N \log N)$ である。

## 注意点

- 2 つの列の長さが異なる場合や、`Modint<MOD>` 上で評価点が重複する場合は `std::invalid_argument` を送出する。
- NTT の変換長が $\mathrm{MOD} - 1$ を割り切る必要がある。
- 空の入力には空の係数の列を返す。

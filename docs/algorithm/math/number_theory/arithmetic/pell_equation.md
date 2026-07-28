---
title: Pell Equation (Pell方程式)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/pell_equation.hpp
---

非平方正整数 $D$ に対する $x^2-Dy^2=1$ の最小正整数解を求める。

## API

```cpp
optional<PellEquationSolution>
math::minimum_pell_equation_solution(long long nonsquare);
```

結果の `x` と `y` は桁数制限のない `BigInteger` である。`nonsquare` が平方数なら `nullopt` を返す。

## 時間計算量

$\sqrt D$ の連分数周期長を $L$、解のビット長を $B$、多倍長乗算時間を $M(B)$ として $O(LM(B))$ 時間、$O(B)$ 空間。

## 注意点

`nonsquare <= 0` では `runtime_error` を送出する。には依存せず、リポジトリ内の `BigInteger` を使用する。

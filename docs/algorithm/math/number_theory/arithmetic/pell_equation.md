---
title: Pell Equation (Pell方程式)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/pell_equation.hpp
---

平方数でない正整数 $D$ に対する Pell 方程式 $x^2-Dy^2=1$ の最小の正整数解を求める。

## API

```cpp
struct PellEquationSolution{ BigInteger x; BigInteger y; };

optional<PellEquationSolution>
math::minimum_pell_equation_solution(long long nonsquare);
```

$\sqrt D$ の連分数展開の収束分数 $x/y$ を順に作り、$x^2-Dy^2=1$ となった最初のものを返す。結果の `x` と `y` は桁数に制限のない `BigInteger` である。`nonsquare` が平方数なら `nullopt` を返す。

## 時間計算量

$\sqrt D$ の連分数の周期長を $L$、解の bit 長を $B$、多倍長乗算の時間を $M(B)$ として $O(LB)$ 時間、$O(B)$ 空間。$k$ 番目の近似分数 $x/y$ は $x^2-Dy^2=(-1)^{k+1}Q_{k+1}$（$Q_{k+1}$ は連分数展開の次の分母）を満たすので、$Q_{k+1}=1$ かつ $k$ が奇数になった最初の近似分数を解とする。多倍長演算は「小さい係数 × 多倍長 + 多倍長」だけである。

## 注意点

- `nonsquare <= 0` では `runtime_error` を送出する。
- 外部ライブラリには依存せず、リポジトリ内の `BigInteger`（`big_integer.hpp`）を使う。
- 周期長 $L$ は $O(\sqrt D \log D)$ まで大きくなりうるため、$D$ によっては解が非常に大きくなる。

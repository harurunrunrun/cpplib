---
title: Value-Bounded Constant-Time GCD (値域前処理定数時間最大公約数)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/constant_time_gcd.hpp
---

値域の上限 $V$ を先に固定し、$0\leq a,b\leq V$ の最大公約数を最悪 $O(1)$ で求める。

各正整数を 3 つの因子に分ける。各因子は $\lfloor\sqrt V\rfloor$ 以下か素数になるよう、線形篩で構成する。小さい因子との GCD は $\sqrt V\times\sqrt V$ の表で、大きい素因子との GCD は 1 回の剰余で判定する。このためクエリは常に 3 因子だけを処理する。

```cpp
class ConstantTimeGcd;
```

## 構築

```cpp
explicit ConstantTimeGcd(std::uint32_t maximum)
```

値域 $[0,\mathrm{maximum}]$ 用の表と 3 因子分解を構築する（$V$ = `maximum`）。

- 時間計算量: $O(V)$
- 空間計算量: $O(V)$

小さい数同士の GCD の表も、クエリごとに Euclid の互除法を呼ばず、計算済みの小さい行から 1 マスずつ求めるため、構築全体も $O(V)$ である。

## `max_value`

```cpp
std::uint32_t max_value() const
```

構築時に指定した値域の上限を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## GCD クエリ

```cpp
std::uint32_t query(std::uint32_t first, std::uint32_t second) const
std::uint32_t operator()(std::uint32_t first, std::uint32_t second) const
```

`gcd(first, second)` を返す。`gcd(0, 0) = 0` とする。どちらかが `max_value()` を超える場合は `std::out_of_range` を送出する。

- 時間計算量: 最悪 $O(1)$（高々 3 回の表引き・素因子の処理）
- 追加空間計算量: $O(1)$

## 注意点

- 入力と上限は `std::uint32_t` である。クエリ数が多く、$O(V)$ の前処理領域を確保できる場合に使う。
- 構築時のメモリ確保に失敗した場合は標準コンテナの例外が送出される。

---
title: Value-Bounded Constant-Time GCD (値域前処理定数時間最大公約数)
documentation_of: ../src/algorithm/math/number_theory/constant_time_gcd.hpp
---

値域上限 $V$ を先に固定し、$0\leq a,b\leq V$ の最大公約数をworst-case
$O(1)$ で求める。

各正整数を3因子に分ける。各因子は $\lfloor\sqrt V\rfloor$ 以下か素数になるよう
線形sieveで構成する。小さい因子とのGCDは $\sqrt V\times\sqrt V$ table、
大きい素因子とのGCDは1回の剰余で判定する。このためqueryは常に3因子だけを処理する。

```cpp
class ConstantTimeGcd;
```

## 構築

```cpp
explicit ConstantTimeGcd(std::uint32_t maximum)
```

値域 $[0,maximum]$ 用のtableと3因子分解を構築する。

- 時間計算量: $O(V)$
- 領域計算量: $O(V)$

小GCD tableもEuclid法をqueryごとに呼ばず、既に計算済みの小さい行から1 cellを
求めるため、構築全体も $O(V)$ である。

## `max_value`

```cpp
std::uint32_t max_value() const
```

構築時に指定した値域上限を返す。

- 時間計算量: $O(1)$
- 追加領域計算量: $O(1)$

## GCD query

```cpp
std::uint32_t query(std::uint32_t first, std::uint32_t second) const
std::uint32_t operator()(std::uint32_t first, std::uint32_t second) const
```

`gcd(first, second)` を返す。`gcd(0,0)=0` とする。どちらかが `max_value()` を
超える場合は `std::out_of_range`。

- 時間計算量: worst-case $O(1)$（高々3回のtable/素因子処理）
- 追加領域計算量: $O(1)$

## 注意点

入力と上限は `std::uint32_t`。query数が多く、$O(V)$ の前処理領域を確保できる
場合に使う。構築時のallocationに失敗した場合は標準containerの例外が送出される。

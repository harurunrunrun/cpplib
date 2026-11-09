---
title: Big Integer GCD (多倍長整数の最大公約数)
documentation_of: ../../../../src/algorithm/math/integer/big_integer_gcd.hpp
---

`BigInteger` に対する Euclid の互除法、拡張 Euclid の互除法、最小公倍数を提供する。

以下、入力の最大 bit 長を $B$、32 bit limb の個数を $L$ とする。`BigInteger::divmod` は 2 進の筆算で、1 回の除算は（商の bit 長 $+1$）$\times O(L)$ かかる。互除法全体で商の bit 長の和は $O(B)$ なので、除算の合計は $O(BL)$ になる。

## `big_integer_gcd`

```cpp
BigInteger big_integer_gcd(BigInteger first, BigInteger second)
```

`first` と `second` の非負の最大公約数を返す。両方 0 なら 0 を返す。

- 時間計算量: $O(BL)$
- 追加空間計算量: $O(L)$

## `big_integer_extended_gcd`

```cpp
struct BigIntegerExtendedGcdResult{
    BigInteger gcd;
    BigInteger x;
    BigInteger y;
};

BigIntegerExtendedGcdResult big_integer_extended_gcd(
    BigInteger first,
    BigInteger second
)
```

`gcd >= 0` かつ `first*x + second*y == gcd` を満たす 3 つ組を返す。`first == second == 0` では `{0, 1, 0}` を返す。

- 時間計算量: $O(BL)$ に係数更新の乗算時間を加えたもの
- 追加空間計算量: $O(L)$

## `big_integer_lcm`

```cpp
BigInteger big_integer_lcm(
    const BigInteger& first,
    const BigInteger& second
)
```

非負の最小公倍数を返す。どちらかが 0 なら 0 を返す。オーバーフローではなく無駄な桁の増加を避けるため、先に GCD で割ってから乗算する。

- 時間計算量: $O(BL + M(L))$（$M(L)$ は `BigInteger` の乗算時間）
- 追加空間計算量: $O(L)$

## 注意点

内部では `BigInteger::divmod` を使うため、除算は 0 方向へ丸める。符号付きの入力でも、GCD と LCM は常に非負である。

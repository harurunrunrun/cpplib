---
title: Big Integer GCD (多倍長整数の最大公約数)
documentation_of: ../src/algorithm/math/big_integer_gcd.hpp
---

`BigInteger` に対するEuclidの互除法、拡張Euclid、最小公倍数を提供する。

## `big_integer_gcd`

```cpp
BigInteger big_integer_gcd(BigInteger first, BigInteger second)
```

`first` と `second` の非負の最大公約数を返す。両方0なら0。

- 時間計算量: 除算回数を $Q$、最大bit長を $B$、32-bit limb数を $L$ として
  $O(QBL)$
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

`gcd >= 0` かつ `first*x + second*y == gcd` を満たす3値を返す。
`first == second == 0` では `{0, 1, 0}`。

- 時間計算量: $O(QBL)$ に係数更新の乗算時間を加えたもの
- 追加空間計算量: $O(L)$

## `big_integer_lcm`

```cpp
BigInteger big_integer_lcm(
    const BigInteger& first,
    const BigInteger& second
)
```

非負の最小公倍数を返す。どちらかが0なら0。先にGCDで割ってから乗算する。

- 時間計算量: $O(QBL+L^2)$
- 追加空間計算量: $O(L)$

## 注意点

内部では `BigInteger::divmod` を使うため、除算の丸め規則は0方向である。
符号付き入力でもGCDとLCMは常に非負。


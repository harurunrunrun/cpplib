---
title: Fast Big Integer (高速多倍長整数)
documentation_of: ../../../../src/algorithm/math/integer/fast_big_integer.hpp
---

符号付き多倍長整数 `ExactInteger` と、10進入出力・四則演算を備えた
`BigInteger` を提供する。64-bit limb、Karatsuba乗算、大きな除算での逆数近似、
大きな10進文字列での分割統治変換を利用する高速版である。

以下で $L$ は64-bit limb数、$D$ は10進桁数、$M(L)$ は乗算時間とする。
均衡した大きな入力の乗算は $M(L)=O(L^{\log_2 3})$、小さい入力と著しく
不均衡な入力ではschoolbook法を使う。

## 構築・代入

```cpp
ExactInteger()
template<class Integer> ExactInteger(Integer value)
BigInteger()
template<class Integer> BigInteger(Integer value)
explicit BigInteger(std::string_view decimal)
BigInteger& assign(std::string_view decimal)
```

組み込み整数または符号付き10進文字列から構築する。文字列は先頭の
`+` / `-` を許す。空文字列、符号だけ、10進数字以外を含む文字列では
`invalid_argument`。

- 整数からの構築: $O(L)$
- 文字列からの構築・代入: 通常は $O(D^2)$、大入力では分割統治により
  $O(M(D)\log D)$
- 空間計算量: $O(L)$

## 状態取得・変換

```cpp
bool is_zero() const
bool is_negative() const
std::size_t bit_length() const
ExactInteger absolute() const
BigInteger absolute() const
std::string to_string() const
template<class Integer> Integer checked_to() const
const ExactInteger& exact_integer() const
```

0判定、符号、bit長、絶対値、10進文字列を取得する。`checked_to` は値が変換先の
範囲外なら `overflow_error`。`exact_integer` は内部値への読み取り専用参照を返す。

- `is_zero`, `is_negative`, `bit_length`, `exact_integer`: $O(1)$
- `absolute`, `checked_to`: $O(L)$
- `to_string`: 小入力では $O(L^2)$、大入力では分割統治により
  $O(M(L)\log L)$

## 四則演算・`divmod`

```cpp
ExactInteger& operator+=(const ExactInteger&)
ExactInteger& operator-=(const ExactInteger&)
ExactInteger& operator*=(const ExactInteger&)
BigInteger& operator+=(const BigInteger&)
BigInteger& operator-=(const BigInteger&)
BigInteger& operator*=(const BigInteger&)
BigInteger& operator/=(const BigInteger&)
BigInteger& operator%=(const BigInteger&)
static std::pair<BigInteger, BigInteger> divmod(
    const BigInteger& dividend,
    const BigInteger& divisor
)
```

対応する非破壊演算 `+`, `-`, `*`, `/`, `%` もある。式オブジェクトにより、代入先へ
直接計算できる組合せでは不要な一時値を抑える。除数0では `domain_error`。
商 $q$ と余り $r$ は `a == q*b + r`、`abs(r) < abs(b)` を満たす。

- 加減算: $O(L)$
- 乗算: $O(M(L))$
- 除算・剰余・`divmod`: 通常のlong divisionでは $O(L^2)$、十分大きい入力では
  逆数近似を利用して $O(M(L)\log L)$
- 追加空間計算量: $O(L)$

## shift・比較・入出力

```cpp
BigInteger operator-() const
BigInteger& operator<<=(std::size_t shift)
BigInteger& operator>>=(std::size_t shift)
BigInteger& operator++()
BigInteger& operator--()
bool operator==(...)
std::strong_ordering operator<=>(...)
std::ostream& operator<<(std::ostream&, const BigInteger&)
std::istream& operator>>(std::istream&, BigInteger&)
```

非破壊shiftと後置増減も提供する。右shiftは負数を $-\infty$ 方向へ丸める。
不正なstream入力では `failbit` を設定し、代入先を変更しない。

- 単項minus、増減: $O(L)$
- shift: $O(L+shift/64)$
- 比較: $O(L)$
- stream入出力: 対応する文字列変換と同じ

## 時間計算量

各APIの時間計算量は上記の各節に示したとおりである。基本操作はlimb数に線形、
乗算は $O(M(L))$、除算と10進変換は入力サイズに応じて二次アルゴリズムと
分割統治アルゴリズムを切り替える。

## 注意点

GCC 13以降のC++20を前提とする。同じ翻訳単位で通常版の `big_integer.hpp` と
同時にincludeしてはならない。両方が同名の `ExactInteger` と `BigInteger` を
提供するため、用途に応じて一方だけを選ぶ。

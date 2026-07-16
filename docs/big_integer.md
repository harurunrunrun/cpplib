---
title: Big Integer (多倍長整数)
documentation_of: ../src/algorithm/math/big_integer.hpp
---

符号付き多倍長整数。`ExactInteger` の加減算・乗算・shiftを利用し、10進文字列の
入出力と多倍長整数同士の除算・剰余を加えた型である。除算は2進long divisionで、
C++の整数除算と同じく0方向へ丸める。

## 構築・代入

```cpp
BigInteger()
template<class Integer> BigInteger(Integer value)
explicit BigInteger(std::string_view decimal)
BigInteger& assign(std::string_view decimal)
```

整数または符号付き10進文字列から作る。文字列は先頭の `+` / `-` を許す。
空文字列、符号だけ、10進数字以外を含む文字列では `invalid_argument`。

- 整数からの構築: $O(W)$
- $D$ 桁の文字列からの構築・代入: $O(D^2)$
- 空間計算量: 値のbit長を $B$ として $O(B)$

## 状態取得

```cpp
bool is_zero() const
bool is_negative() const
std::size_t bit_length() const
BigInteger absolute() const
std::string to_string() const
const ExactInteger& exact_integer() const
```

0判定、符号、絶対値のbit長、絶対値、10進文字列、内部の読み取り専用
`ExactInteger` を返す。

- `is_zero`, `is_negative`: $O(1)$
- `bit_length`: $O(1)$
- `absolute`, `exact_integer`: それぞれ $O(B)$, $O(1)$
- `to_string`: $O(B^2)$

## `checked_to`

```cpp
template<class Integer> Integer checked_to() const
```

組み込み整数型へ変換する。範囲外なら `overflow_error`。

- 時間計算量: $O(B)$
- 追加空間計算量: $O(1)$

## 四則演算

```cpp
BigInteger& operator+=(const BigInteger&)
BigInteger& operator-=(const BigInteger&)
BigInteger& operator*=(const BigInteger&)
BigInteger& operator/=(const BigInteger&)
BigInteger& operator%=(const BigInteger&)
```

対応する非破壊演算 `+`, `-`, `*`, `/`, `%` もある。除数0では `domain_error`。
除算の商 $q$ と余り $r$ は `a == q*b + r`、`abs(r) < abs(b)` を満たし、
0でない余りの符号は `a` と同じ。

bit長を $B$、32-bit limb数を $L$ とする。

- 加減算: $O(L)$
- 乗算: $O(L^2)$
- 除算・剰余: $O(BL)$
- 空間計算量: $O(L)$

## `divmod`

```cpp
static std::pair<BigInteger, BigInteger> divmod(
    const BigInteger& dividend,
    const BigInteger& divisor
)
```

商と余りを1回のlong divisionで返す。除数0では `domain_error`。

- 時間計算量: $O(BL)$
- 追加空間計算量: $O(L)$

## shift・単項演算・増減

```cpp
BigInteger operator-() const
BigInteger& operator<<=(std::size_t shift)
BigInteger& operator>>=(std::size_t shift)
BigInteger& operator++()
BigInteger operator++(int)
BigInteger& operator--()
BigInteger operator--(int)
```

非破壊shiftもある。右shiftは負数を $-\infty$ 方向へ丸める算術shift。

- 単項minus、増減: $O(L)$
- shift: $O(L+shift/32)$
- 追加空間計算量: $O(L+shift/32)$

## 比較・stream入出力

```cpp
bool operator==(...)
std::strong_ordering operator<=>(...)
std::ostream& operator<<(std::ostream&, const BigInteger&)
std::istream& operator>>(std::istream&, BigInteger&)
```

通常の符号付き整数順で比較する。入力が不正な10進文字列ならstreamの
`failbit` を設定し、代入先を変更しない。

- 比較: $O(L)$
- 入力: 10進桁数 $D$ に対して $O(D^2)$
- 出力: $O(B^2)$

## 注意点

保存領域は値のbit長に比例する。除算は巨大乗算向けのFFTやHalf-GCDを使用しない
2進long divisionである。


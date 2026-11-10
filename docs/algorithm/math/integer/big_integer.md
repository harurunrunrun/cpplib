---
title: Big Integer (多倍長整数)
documentation_of: ../../../../src/algorithm/math/integer/big_integer.hpp
---

符号付き多倍長整数。`ExactInteger` の加減算・乗算・シフトを利用し、10 進文字列の入出力と多倍長整数同士の除算・剰余を加えた型である。除算は 2 進の筆算（long division）で、C++ の整数除算と同じく 0 方向へ丸める。

以下、値の bit 長を $B$、32 bit limb の個数を $L$（$L \approx B/32$）、10 進桁数を $D$ とする。乗算の計算量 $M(L)$ は `ExactInteger` と同じで、長さに応じて筆算・Karatsuba 法・Toom-Cook-3 法・NTT を切り替え、NTT の対応範囲では $O(L \log L)$ である。

## 構築・代入

```cpp
BigInteger()
template<class Integer> BigInteger(Integer value)
explicit BigInteger(std::string_view decimal)
explicit BigInteger(const ExactInteger& value)
explicit BigInteger(ExactInteger&& value)
BigInteger& assign(std::string_view decimal)
```

組み込み整数、`ExactInteger`、または符号付き 10 進文字列から作る。文字列は先頭の `+` / `-` を許す。空文字列、符号だけの文字列、10 進数字以外を含む文字列では `invalid_argument` を送出する。

- 整数からの構築: $O(1)$
- $D$ 桁の文字列からの構築・代入: $O(D^2)$（1 桁ずつ 10 倍して加える）
- 空間計算量: $O(B)$

## 状態取得

```cpp
bool is_zero() const
bool is_negative() const
std::size_t bit_length() const
BigInteger absolute() const
std::string to_string() const
const ExactInteger& exact_integer() const
```

それぞれ 0 判定、負であるかの判定、絶対値の bit 長、絶対値、10 進文字列、内部の `ExactInteger`（読み取り専用）を返す。非メンバ関数 `abs(value)` も絶対値を返す。

- `is_zero`, `is_negative`, `bit_length`, `exact_integer`: $O(1)$
- `absolute`: $O(L)$
- `to_string`: `ExactInteger::to_string` と同じで $O(M(L) \log L + D)$（NTT の対応範囲では $O(L \log^2 L + D)$）

## `checked_to`

```cpp
template<class Integer> Integer checked_to() const
```

組み込み整数型へ変換する。範囲外なら `overflow_error` を送出する。

- 時間計算量: $O(L)$
- 追加空間計算量: $O(1)$

## 四則演算

```cpp
BigInteger& operator+=(const BigInteger&)
BigInteger& operator-=(const BigInteger&)
BigInteger& operator*=(const BigInteger&)
BigInteger& operator/=(const BigInteger&)
BigInteger& operator%=(const BigInteger&)
```

対応する非破壊演算 `+`, `-`, `*`, `/`, `%` もある。除数が 0 なら `domain_error` を送出する。除算の商 $q$ と余り $r$ は `a == q*b + r` と `abs(r) < abs(b)` を満たし、0 でない余りの符号は `a` と同じである。

- 加減算: $O(L)$
- 乗算: $O(M(L))$
- 除算・剰余: $O(BL)$（被除数と除数の bit 長の差を $s$ として、$s + 1$ 回の比較・減算・シフト）
- 空間計算量: $O(L)$

## `divmod`

```cpp
static std::pair<BigInteger, BigInteger> divmod(
    const BigInteger& dividend,
    const BigInteger& divisor
)
```

商と余りを 1 回の筆算で返す。除数が 0 なら `domain_error` を送出する。

- 時間計算量: $O(BL)$
- 追加空間計算量: $O(L)$

## シフト・単項演算・増減

```cpp
BigInteger operator-() const
BigInteger& operator<<=(std::size_t shift)
BigInteger& operator>>=(std::size_t shift)
BigInteger& operator++()
BigInteger operator++(int)
BigInteger& operator--()
BigInteger operator--(int)
```

非破壊のシフト `<<`, `>>` もある。右シフトは負数を $-\infty$ 方向へ丸める算術シフトである。

- 単項マイナス、増減: $O(L)$
- シフト: $O(L+\mathrm{shift}/32)$
- 追加空間計算量: $O(L+\mathrm{shift}/32)$

## 比較・ストリーム入出力

```cpp
bool operator==(...)
std::strong_ordering operator<=>(...)
std::ostream& operator<<(std::ostream&, const BigInteger&)
std::istream& operator>>(std::istream&, BigInteger&)
```

通常の符号付き整数の順序で比較する。入力で空白区切りのトークンが不正な 10 進文字列なら、ストリームに `failbit` を設定し、代入先は変更しない。

- 比較: $O(L)$
- 入力: $O(D^2)$
- 出力: `to_string` と同じ

## 注意点

保存領域は値の bit 長に比例する。除算は Newton 法や Half-GCD などの高速な手法を使わない 2 進の筆算なので、巨大な数同士の除算は遅い（高速な除算が必要なら `FastBigInteger` を参照）。

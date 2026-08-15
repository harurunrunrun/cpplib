---
title: Fast Big Integer (高速多倍長整数) [addition_of_big_integers] [division_of_big_integers] [multiplication_of_big_integers]
documentation_of: ../../../../src/algorithm/math/integer/fast_big_integer.hpp
---

10進入出力と符号付き四則演算に特化した `BigInteger` を提供する。
内部表現は符号と little-endian の base 10^9 uint32_t limb であり、
4 limbまではオブジェクト内に保持する。外部多倍長整数ライブラリには依存せず、GCC 13、
C++20、`__uint128_t`を前提とする。

## 構築・代入

```cpp
BigInteger()
template<class Integer> BigInteger(Integer value)
explicit BigInteger(std::string_view decimal)
template<class Integer> BigInteger& operator=(Integer value)
BigInteger& assign(std::string_view decimal)
```

組み込み整数または符号付き10進文字列から構築する。文字列では先頭の
`+` / `-` と先頭0を許す。空文字列、符号だけの文字列、10進数字以外を
含む文字列では `std::invalid_argument` を送出する。入力は下位から9桁ずつ
直接limbへ変換するため、10進桁数に対して線形時間である。

0は常に非負へ正規化される。

## 状態取得・変換

```cpp
bool is_zero() const noexcept
bool is_negative() const noexcept
BigInteger absolute() const
std::string to_string() const
template<class Integer> Integer checked_to() const
```

`to_string` は最上位limbを通常幅、そのほかを9桁固定幅で直接連結し、
10進桁数に対して線形時間で動作する。`checked_to<Integer>()` は値が変換先の
組み込み整数型に収まらない場合に `std::overflow_error` を送出する。

非メンバー関数 `abs(value)` も利用できる。

## 四則演算

```cpp
BigInteger& operator+=(const BigInteger&)
BigInteger& operator-=(const BigInteger&)
BigInteger& operator*=(const BigInteger&)
BigInteger& operator/=(const BigInteger&)
BigInteger& operator%=(const BigInteger&)

BigInteger operator+(const BigInteger&, const BigInteger&)
BigInteger operator-(const BigInteger&, const BigInteger&)
BigInteger operator*(const BigInteger&, const BigInteger&)
BigInteger operator/(const BigInteger&, const BigInteger&)
BigInteger operator%(const BigInteger&, const BigInteger&)
```

加減算はlimb数に対して線形である。乗算は短い側が128 limb以下なら
schoolbook法、それより長ければ次の3素数上のMontgomery radix-4 NTTとCRTを
使用する。

- `167772161`
- `469762049`
- `754974721`

CRTで各係数を __uint128_t に復元してから base 10^9 のcarryを
処理する。同じオブジェクトの二乗ではforward transformを1回に省略する。
必要なNTT長が 2^24 を超える場合は std::length_error を送出する。

## 除算と `divmod`

```cpp
static std::pair<BigInteger, BigInteger> divmod(
    const BigInteger& dividend,
    const BigInteger& divisor
)
```

1-limb除算、除数64 limb以下の正規化long division、それより大きい場合の
Newton逆数法を切り替える。`divmod` は商と余りを同時に求めるので、両方が
必要な場合に `/` と `%` を個別に呼ぶより効率がよい。

除数0では `std::domain_error` を送出する。商は0方向へ丸め、余りは被除数と
同符号である。返される商 `q` と余り `r` は常に次を満たす。

```text
dividend == q * divisor + r
abs(r) < abs(divisor)
```

大きな均衡入力の除算は、乗算時間を M(n) として O(M(n) log n) 時間である。

## 比較・符号・入出力

```cpp
BigInteger operator-() const
BigInteger& operator++()
BigInteger& operator--()
bool operator==(const BigInteger&, const BigInteger&)
std::strong_ordering operator<=>(const BigInteger&, const BigInteger&)
std::ostream& operator<<(std::ostream&, const BigInteger&)
std::istream& operator>>(std::istream&, BigInteger&)
```

後置の `++` / `--` も提供する。stream入力が不正な10進文字列なら
`failbit` を設定し、代入先の値を変更しない。

## 時間計算量

10進入出力と加減算はlimb数に対して線形である。schoolbook乗算は二次時間、
NTT乗算は O(n log n)、Newton除算は O(M(n) log n) 時間で動作する。
追加空間は大きな演算で O(n) である。

## 注意点

この高速版は10進Big Integer問題向けのAPIである。旧版に存在した
`ExactInteger`、`exact_integer()`、`bit_length()`、bit shift、
式テンプレート型は提供しない。16進入出力が必要な用途では別実装を使用する。

---
title: Rational (有理数)
documentation_of: ../../../../src/structure/types/rational/rational.hpp
---

任意精度整数の既約分数として有理数を保持する。分母は常に正で、0 は `0/1`
に正規化される。浮動小数点数からの構築は許可しない。

## 型と構築

```cpp
cp::Rational()
cp::Rational(integral value)
cp::Rational(integral numerator, integral denominator)
explicit cp::Rational(string_view text)
cp::Rational(string_view numerator, string_view denominator)
```

文字列は 10 進整数または `numerator/denominator` 形式で渡す。分母が 0、不正な文字列、
複数の `/` を含む文字列は例外を送出する。整数型以外（浮動小数点数）からの構築は `delete` されている。

## 値の取得と変換

```cpp
string numerator_string() const
string denominator_string() const
int sign() const
bool is_zero() const
bool is_integer() const
explicit operator bool() const
string to_string() const
string to_decimal(size_t digits, bool trim_trailing_zeros = false) const
long double to_long_double() const
size_t hash_value() const
```

`to_decimal` は小数点以下を丸めずに切り捨てる。`to_long_double` は明示的な近似変換で
あり、正確な判定には使わない。`std::hash<cp::Rational>` も提供する。

## 算術演算

```cpp
Rational operator+() const
Rational operator-() const
Rational abs() const
Rational reciprocal() const
Rational& operator+=(const Rational& value)
Rational& operator-=(const Rational& value)
Rational& operator*=(const Rational& value)
Rational& operator/=(const Rational& value)
Rational& operator%=(const Rational& value)
Rational operator+(Rational, const Rational&)
Rational operator-(Rational, const Rational&)
Rational operator*(Rational, const Rational&)
Rational operator/(Rational, const Rational&)
Rational operator%(Rational, const Rational&)
```

複合代入と二項演算は整数型との組合せも両方向に提供する。`++`, `--` も使える。
剰余は `a - trunc(a / b) * b` で定義する。0 による除算・剰余と 0 の逆数は
`std::domain_error` を送出する。

## 比較

```cpp
bool operator==(const Rational&, const Rational&)
bool operator!=(const Rational&, const Rational&)
bool operator<(const Rational&, const Rational&)
bool operator>(const Rational&, const Rational&)
bool operator<=(const Rational&, const Rational&)
bool operator>=(const Rational&, const Rational&)
```

整数型との比較も両方向に提供する。浮動小数点数との比較は提供しない。

## 整数化と分数部分

```cpp
Rational trunc() const
Rational floor() const
Rational ceil() const
Rational round() const
Rational round_ties_to_even() const
Rational fractional_part() const
Rational trunc_fractional_part() const
```

`round` はちょうど半分のとき 0 から遠ざかる向きに丸め、`round_ties_to_even` は偶数へ丸める。
`fractional_part` は `[0, 1)`、`trunc_fractional_part` は元の値と同符号になる。

## べき乗・連分数

```cpp
Rational pow(int64_t exponent) const
vector<Rational> continued_fraction() const
static Rational from_continued_fraction(const vector<Rational>& terms)
static Rational mediant(const Rational& left, const Rational& right)
```

0 の負の指数乗は `std::domain_error` を、空または整数でない項を含む連分数からの構築は
`std::invalid_argument` を送出する。自由関数 `abs`, `floor`, `ceil`, `trunc`,
`round`, `pow` も `cp` 名前空間にある。

## 入出力

```cpp
ostream& operator<<(ostream&, const Rational&)
istream& operator>>(istream&, Rational&)
```

出力は既約な整数または分数形式である。不正な入力ではストリームの `failbit` を立てる。

## API 別の時間計算量・空間計算量

分子・分母の最大の語数（多倍長整数の内部の桁数）を $L$、その乗算時間を $M(L)$ とする。この実装の
$M(L)$ は小さい入力で $O(L^2)$、閾値以上で Karatsuba 法の $O(L^{\log_2 3})$ である。
また、べき乗の指数の絶対値を $E$ とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `sign`, `is_zero`, `is_integer`, `operator bool` | $O(1)$ | $O(1)$ |
| 文字列からの構築、`to_string` | $O(L^2)$ | $O(L)$ |
| `numerator_string`, `denominator_string` | $O(L^2)$ | $O(L)$ |
| `to_decimal(digits)` | $O((1 + \mathit{digits} / 9)L^2)$ | $O(L + \mathit{digits})$ |
| `to_long_double`, `hash_value` | $O(L)$ | $O(1)$ |
| 比較 | $O(M(L))$ | $O(L)$ |
| 加減算・乗算 | $O(M(L) + L^2)$ | $O(L)$ |
| 除算・剰余 | $O(L^2)$ | $O(L)$ |
| `trunc`, `floor`, `ceil`, 各丸め | $O(L^2)$ | $O(L)$ |
| `pow(exponent)` | $O(M(L)\log(E + 1) + L^2)$ | $O(L)$ |
| `continued_fraction` | ユークリッドの互除法の除算列に比例 | $O(L)$（戻り値を除く） |
| `from_continued_fraction` | 項からの乗加算列に比例 | $O(L)$（戻り値を除く） |
| `mediant` | $O(L^2)$ | $O(L)$ |

## 注意点

- GCC 13 の C++20 で利用する。外部の多倍長整数ライブラリには依存しない。
- 内部整数型は公開 API ではない。分子・分母は文字列で取得する。
- 巨大な値の浮動小数点近似はオーバーフローや精度低下を起こし得る。
- 分母は必ず正に正規化されるため、等値比較は正規化済み表現の比較である。

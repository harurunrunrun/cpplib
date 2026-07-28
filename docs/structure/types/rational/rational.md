---
title: Rational (有理数)
documentation_of: ../../../../src/structure/types/rational/rational.hpp
---

任意精度整数の既約分数として有理数を保持する。分母は常に正で、0は `0/1`
へ正規化される。浮動小数点数からの構築は許可しない。

## 型と構築

```cpp
cp::Rational()
cp::Rational(integral value)
cp::Rational(integral numerator, integral denominator)
explicit cp::Rational(string_view text)
cp::Rational(string_view numerator, string_view denominator)
```

文字列は10進整数または `numerator/denominator` 形式で渡す。分母0、不正な文字列、
複数の `/` は例外になる。

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
剰余は `a - trunc(a / b) * b` で定義する。0による除算・剰余と0の逆数は
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

`round` はちょうど半分を0から遠ざけ、`round_ties_to_even` は偶数へ丸める。
`fractional_part` は `[0, 1)`、`trunc_fractional_part` は元の値と同符号になる。

## べき乗・連分数

```cpp
Rational pow(int64_t exponent) const
vector<Rational> continued_fraction() const
static Rational from_continued_fraction(const vector<Rational>& terms)
static Rational mediant(const Rational& left, const Rational& right)
```

負指数の0乗算は `std::domain_error`、空または非整数項を含む連分数は
`std::invalid_argument` になる。自由関数 `abs`, `floor`, `ceil`, `trunc`,
`round`, `pow` も `cp` 名前空間にある。

## 入出力

```cpp
ostream& operator<<(ostream&, const Rational&)
istream& operator>>(istream&, Rational&)
```

出力は既約な整数または分数形式である。不正入力ではstreamの `failbit` を立てる。

## API別の時間計算量・空間計算量

分子・分母の最大limb数を $L$、limb乗算時間を $M(L)$ とする。この実装の
`M(L)` は小入力で $O(L^2)$、閾値以上でKaratsuba法の $O(L^{\log_2 3})$ である。
また、べき乗の指数の絶対値を $E$ とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `sign`, `is_zero`, `is_integer`, `operator bool` | $O(1)$ | $O(1)$ |
| 文字列からの構築、`to_string` | $O(L^2)$ | $O(L)$ |
| `numerator_string`, `denominator_string` | $O(L^2)$ | $O(L)$ |
| `to_decimal(digits)` | $O((1 + digits / 9)L^2)$ | $O(L + digits)$ |
| `to_long_double`, `hash_value` | $O(L)$ | $O(1)$ |
| 比較 | $O(M(L))$ | $O(L)$ |
| 加減算・乗算 | $O(M(L) + L^2)$ | $O(L)$ |
| 除算・剰余 | $O(L^2)$ | $O(L)$ |
| `trunc`, `floor`, `ceil`, 各丸め | $O(L^2)$ | $O(L)$ |
| `pow(exponent)` | $O(M(L)\log(E + 1) + L^2)$ | $O(L)$ |
| `continued_fraction` | Euclid法の除算列に比例 | $O(L)$（戻り値を除く） |
| `from_continued_fraction` | 項からの乗加算列に比例 | $O(L)$（戻り値を除く） |
| `mediant` | $O(L^2)$ | $O(L)$ |

## 注意点

- GCC 13のC++20で利用する。Boostには依存しない。
- 内部整数型は公開APIではない。分子・分母は文字列で取得する。
- 巨大値の浮動小数点近似はoverflowまたは精度低下を起こし得る。
- 分母は必ず正に正規化されるため、等値比較は正規化済み表現の比較である。

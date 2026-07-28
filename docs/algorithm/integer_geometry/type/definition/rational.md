---
title: Rational Number (有理数)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/rational.hpp
---

`ExactInteger` の分子・正の分母を既約化して保持する厳密有理数 `integer_geometry::Rational` を提供する。

## API

- `Rational()`, `Rational(integer)`, `Rational(numerator, denominator)`: 0、整数、分数から構築する。
- `numerator()`, `denominator()`: 正規化済みの分子と分母を参照する。
- `is_zero()`, `sign()`: 0判定と符号を返す。
- `reciprocal()`: 逆数を返す。
- 単項 `+`, `-`、`+=`, `-=`, `*=`, `/=` と対応する二項演算: 厳密な有理数演算を行う。
- `operator==`, `operator<=>`: 厳密比較を行う。
- `to_string()`, `operator<<`: 整数または `numerator/denominator` 形式で出力する。
- `abs(value)`: 絶対値を返す。

## API別の時間計算量・空間計算量

最大bit長を $L$、`ExactInteger` の乗算時間を $M(L)$、任意精度除算・最大公約数を含む正規化時間を $N(L)$ とする。

- 既定構築、`numerator`、`denominator`、`is_zero`、`sign`: 時間 $O(1)$、追加領域 $O(1)$。
- 整数構築、単項演算、`abs`: 時間・領域ともに $O(L)$。
- 分数構築、`reciprocal`、四則演算: 時間 $O(N(L)+M(L))$、追加領域 $O(L)$。
- 比較: 時間 $O(M(L))$、追加領域 $O(L)$。
- `to_string` と出力: `ExactInteger` の十進変換時間、出力長 $O(L)$ の領域。

## 注意点

分母0の構築は `std::invalid_argument`、0の逆数または0除算は `std::domain_error` を送出する。分母は常に正で、0は `0/1` として保持する。

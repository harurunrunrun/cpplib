---
title: Rational Number (有理数)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/rational.hpp
---

`ExactInteger` の分子と正の分母を既約な形で保持する厳密有理数 `integer_geometry::Rational` を提供する。あわせて `integer_geometry::Integer` を `ExactInteger` の別名として定義する。

## API

- `Rational()`: $0$ を構築する。
- `Rational(value)`: 組み込み整数型の値から構築する。
- `Rational(numerator, denominator = 1)`: `Integer` の分子・分母から構築し、既約化する。
- `numerator()`, `denominator()`: 正規化済みの分子と分母を参照で返す。
- `is_zero()`, `sign()`: $0$ かどうかと、符号（$-1, 0, 1$）を返す。
- `reciprocal()`: 逆数を返す。
- 単項 `+`, `-`、`+=`, `-=`, `*=`, `/=` と対応する二項演算: 厳密な有理数演算を行う。
- `operator==`, `operator<=>`: 厳密に比較する。
- `to_string()`, `operator<<`: 分母が $1$ なら整数として、そうでなければ `numerator/denominator` の形式で出力する。
- `abs(value)`: 絶対値を返す。

## 時間計算量・空間計算量

最大ビット長を $L$、`ExactInteger` の乗算時間を $M(L)$、任意精度除算と最大公約数を含む正規化の時間を $N(L)$ とする。

- デフォルト構築、組み込み整数からの構築、`numerator`、`denominator`、`is_zero`、`sign`: 時間 $O(1)$、追加領域 $O(1)$。
- 単項 `+`: 時間・領域ともに $O(L)$。
- 単項 `-`、`abs`、分子・分母からの構築、`reciprocal`、四則演算: 時間 $O(N(L)+M(L))$、追加領域 $O(L)$（単項 `-`、`abs`、`reciprocal` も内部で再び正規化するため）。
- 比較 `operator<=>`: 時間 $O(M(L))$、追加領域 $O(L)$。`operator==` は分子・分母の比較で時間 $O(L)$。
- `to_string` と出力: `ExactInteger` の十進変換の時間と、出力長 $O(L)$ の領域。

## 注意点

分母が $0$ の構築は `std::invalid_argument` を、$0$ の逆数や $0$ による除算は `std::domain_error` を送出する。分母は常に正であり、$0$ は `0/1` として保持する。

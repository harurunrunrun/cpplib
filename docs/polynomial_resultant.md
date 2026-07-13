---
title: Polynomial Resultant
documentation_of: ../src/algorithm/math/polynomial_resultant.hpp
---

体上の多項式の剰余・終結式・判別式を、固定最大容量の`std::array`で求める。
係数は定数項から昇順に格納し、`coefficient_count`より後ろは参照しない。
型`T`は四則演算、単位元の構築、等値比較を持ち、0でない要素の除算ができる体でなければならない。

## `PolynomialRemainderResult`

```cpp
template<class T, std::size_t CAPACITY>
struct PolynomialRemainderResult {
    std::array<T, CAPACITY> coefficients;
    std::size_t coefficient_count;
};
```

`coefficients`は剰余の係数、`coefficient_count`は末尾の0を除いた有効係数数である。

## `polynomial_remainder`

```cpp
auto remainder = polynomial_remainder(
    dividend, dividend_count, divisor, divisor_count
);
```

`dividend`を`divisor`で割った剰余を返す。0多項式による除算では
`std::invalid_argument`、係数数が`CAPACITY`を超える場合は`std::runtime_error`を送出する。

## `polynomial_resultant`

```cpp
T value = polynomial_resultant(left, left_count, right, right_count);
```

Euclidの互除法に基づいて2多項式の終結式を返す。どちらかが0多項式なら0を返す。
両方が定数多項式なら、0次のSylvester行列の行列式として1を返す。
係数数が`CAPACITY`を超える場合は`std::runtime_error`を送出する。

## `polynomial_discriminant`

```cpp
T value = polynomial_discriminant(coefficients, coefficient_count);
```

多項式の判別式を返す。定数多項式では1、0多項式では
`std::invalid_argument`を返す。係数数が`CAPACITY`を超える場合は
`std::runtime_error`を送出する。

## 計算量

有効な被除数次数を$M$、除数次数を$N$、判別式の入力次数を$D$とする。

| API・操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `PolynomialRemainderResult`の初期化 | $O(\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_remainder` | $O((M-N+1)N+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_resultant` | $O(MN+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_discriminant` | $O(D^2+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| member参照 | $O(1)$ | $O(1)$ |

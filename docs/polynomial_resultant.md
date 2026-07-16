---
title: Polynomial Resultant (多項式終結式)
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

2多項式の終結式を返す。どちらかが0多項式なら0を返す。
両方が定数多項式なら、0次のSylvester行列の行列式として1を返す。
係数数が`CAPACITY`を超える場合は`std::runtime_error`を送出する。

`T = Modint<MOD>`であり、`MOD`が素数かつ必要な2冪長のNTTを
持つ場合は、half-GCDでEuclid変換をまとめて適用する高速経路を使う。
各変換について、先頭係数の冪と次数交換による符号を同時に集約するため、
終結式の値は通常のEuclid法と一致する。例えば`Modint998244353`では、
有効係数数が$2^{22}$以下ならこの条件を満たす。

上記の積を利用できない係数型・法・入力長では、同じAPIが通常の
多項式除算による互換経路を使う。

## `polynomial_discriminant`

```cpp
T value = polynomial_discriminant(coefficients, coefficient_count);
```

多項式の判別式を返す。定数多項式では1、0多項式では
`std::invalid_argument`を返す。係数数が`CAPACITY`を超える場合は
`std::runtime_error`を送出する。

## API別の時間計算量・空間計算量

有効な被除数次数を$A$、除数次数を$B$、終結式・判別式の最大入力次数を
$D$とする。$mathcal M(D)$を、係数体上で次数$D$以下の多項式を
乗算する時間とする。この実装のNTT経路では
$mathcal M(D)=O(D\log D)$である。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `PolynomialRemainderResult`の初期化 | $O(\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_remainder` | $O((A-B+1)B+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_resultant`（高速経路） | $O(\mathcal M(D)\log D)$ | $O(D)$ |
| `polynomial_resultant`（互換経路） | $O(D^2+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_discriminant` | 高速経路では$O(\mathcal M(D)\log D+\mathrm{CAPACITY})$、互換経路では$O(D^2+\mathrm{CAPACITY})$ | $O(D+\mathrm{CAPACITY})$ |
| member参照 | $O(1)$ | $O(1)$ |

## 注意点

係数型は体でなければならない。特に0でない除数の先頭係数が逆元を
持たない場合、結果は定義されないか、係数型の除算が例外を送出する。
高速経路は素数法であることも検査する。入力係数数は`CAPACITY`以下で
なければならない。整数型を係数に使う場合、除算の切り捨てや中間値の
overflowをこのライブラリは補償しない。

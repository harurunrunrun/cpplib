---
title: Polynomial Resultant (多項式終結式)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_resultant.hpp
---

体上の多項式の剰余・終結式・判別式を求める。
固定最大容量の `std::array` を使う固定長版と、実行時サイズに合わせた `std::vector` を使う可変長版がある（どちらもグローバル名前空間）。
係数は定数項から昇冪順に格納する。固定長版では `coefficient_count` より後ろの要素は参照しない。
型 `T` は四則演算、整数からの構築、等値比較をもち、$0$ でない要素で除算できる体でなければならない。

## `PolynomialRemainderResult`

```cpp
template<class T, std::size_t CAPACITY>
struct PolynomialRemainderResult {
    std::array<T, CAPACITY> coefficients;
    std::size_t coefficient_count;
};
```

`coefficients` は剰余の係数、`coefficient_count` は末尾の $0$ を除いた有効な係数の個数である。
テンプレート引数 `CAPACITY` を省略した `PolynomialRemainderResult<T>` では、`coefficients` が `std::vector<T>` になる。

## `polynomial_remainder`

```cpp
auto remainder = polynomial_remainder(
    dividend, dividend_count, divisor, divisor_count
);
auto remainder2 = polynomial_remainder(dividend_vector, divisor_vector);  // 可変長版
```

`dividend` を `divisor` で割った剰余を返す。零多項式で割ると `std::invalid_argument` を送出する。
固定長版で係数の個数が `CAPACITY` を超える場合は `std::runtime_error` を送出する。

## `polynomial_resultant`

```cpp
T value = polynomial_resultant(left, left_count, right, right_count);
T value2 = polynomial_resultant(left_vector, right_vector);  // 可変長版
```

2 つの多項式の終結式を返す。どちらかが零多項式なら $0$ を返す。
両方が定数多項式なら、$0$ 次の Sylvester 行列の行列式として $1$ を返す。
固定長版で係数の個数が `CAPACITY` を超える場合は `std::runtime_error` を送出する。

`T = Modint<MOD>` で、`MOD` が素数かつ必要な長さ（2 の冪）の NTT が可能な場合は、half-GCD で Euclid 互除法の変換をまとめて適用する高速経路を使う。
各変換について先頭係数の冪と、次数の入れ替えによる符号をあわせて集約するため、終結式の値は通常の Euclid 互除法の結果と一致する。
例えば `Modint998244353` では、有効な係数の個数が $2^{22}$ 以下ならこの条件を満たす。

この条件を満たさない係数型・法・入力長では、同じ API が通常の多項式除算による互換経路を使う。

## `polynomial_discriminant`

```cpp
T value = polynomial_discriminant(coefficients, coefficient_count);
T value2 = polynomial_discriminant(coefficients_vector);  // 可変長版
```

多項式の判別式を返す。定数多項式では $1$ を返し、零多項式では `std::invalid_argument` を送出する。
固定長版で係数の個数が `CAPACITY` を超える場合は `std::runtime_error` を送出する。

## 使用例

```cpp
std::vector<Mint> f{-1, 1}, g{-2, 1};
auto value = polynomial_resultant(f, g);
std::array<Mint, 2> fixed_f{-1, 1}, fixed_g{-2, 1};
auto fixed_value = polynomial_resultant(fixed_f, fixed_f.size(), fixed_g, fixed_g.size());
```

## API別の時間計算量・空間計算量

有効な被除数の次数を $A$、除数の次数を $B$、終結式・判別式の入力の最大次数を $D$ とする。
係数体上で次数 $D$ 以下の多項式の乗算にかかる時間を $\mathcal{M}(D)$ とする。この実装の NTT 経路では $\mathcal{M}(D) = O(D \log D)$ である。

固定長版は配列全体をコピー・初期化するため、下表の $O(\mathrm{CAPACITY})$ の項がかかる。可変長版ではこの項が消え、領域は入力長に比例する。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `PolynomialRemainderResult` の初期化 | $O(\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_remainder` | $O((A-B+1)B+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_resultant`（高速経路） | $O(\mathcal{M}(D)\log D)$ | $O(D)$ |
| `polynomial_resultant`（互換経路） | $O(D^2+\mathrm{CAPACITY})$ | $O(\mathrm{CAPACITY})$ |
| `polynomial_discriminant` | 高速経路では $O(\mathcal{M}(D)\log D+\mathrm{CAPACITY})$、互換経路では $O(D^2+\mathrm{CAPACITY})$ | $O(D+\mathrm{CAPACITY})$ |
| メンバ参照 | $O(1)$ | $O(1)$ |

## 注意点

係数型は体でなければならない。特に、$0$ でない除数の先頭係数が逆元をもたない場合、結果は未定義になるか、係数型の除算が例外を送出する。
高速経路は法が素数であることもコンパイル時に検査する。固定長版の入力の係数の個数は `CAPACITY` 以下でなければならない。
整数型を係数に使う場合、除算の切り捨てや中間値のオーバーフローはこのライブラリでは補償しない。

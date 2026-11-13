---
title: Fast Modint (高速剰余整数)
documentation_of: ../../../src/structure/modint/fast_modint.hpp
---

法 `mod` を固定した剰余整数の型。乗算に Barrett reduction を使う。

## テンプレート引数

```cpp
FastModint<mod>
```

$\mathrm{mod} \ge 1$ が必要である。値は常に $[0, \mathrm{mod})$ に正規化される。

## 関数

```cpp
FastModint()
FastModint(long long value)
static constexpr int get_mod()
static FastModint raw(int value)
int val() const
FastModint pow(long long exponent) const
FastModint inv() const
```

- 整数からのコンストラクタは $\mathrm{value} \bmod \mathrm{mod}$ を作り、`val()` は正規化済みの代表値を返す。
- `raw(value)` は正規化を省略するので、$0 \le \mathrm{value} \lt \mathrm{mod}$ は検査されない前提である。
- `pow` は非負の `exponent` 乗を返す。
- `inv` は乗法逆元を返す。除算も右辺の `inv()` を使う。
- ストリームからの入力は符号付き 64 bit 整数として読む。ストリームへの出力は `val()` を 10 進で出力する。

## 演算

```cpp
単項 +, 単項 -, ++, --, +, -, *, /,
+=, -=, *=, /=, ==, !=, operator>>, operator<<
```

- `operator+=(rhs)`: `rhs` を加え、$[0, \mathrm{mod})$ に戻す。
- `operator-=(rhs)`: `rhs` を引き、$[0, \mathrm{mod})$ に戻す。
- `operator*=(rhs)`: Barrett reduction で積を正規化する。
- `operator/=(rhs)`: `rhs.inv()` を掛ける。
- 単項 `operator+()`: 同じ値を返す。単項 `operator-()`: 加法逆元を返す。
- 前置 `operator++()` / `operator--()`: $1$ を加減し、$0$ と $\mathrm{mod}-1$ の間で巡回する。
- 後置 `operator++(int)` / `operator--(int)`: 変更前の値を返してから $1$ を加減する。
- 二項 `operator+`、`operator-`、`operator*`、`operator/`: 左辺のコピーに対応する複合代入演算を行う。
- `operator==` / `operator!=`: 正規化済みの代表値を比較する。
- `operator>>(istream, x)`: 符号付き 64 bit 整数を読み、`x` に正規化して代入する。
- `operator<<(ostream, x)`: `x.val()` を 10 進で出力する。

複合代入演算と前置のインクリメント・デクリメントは `*this` への参照を返し、後置のインクリメント・デクリメントと二項演算は値を返す。

## エイリアス

```cpp
FastModint998244353
FastModint1000000007
```

## 時間計算量

入出力については、整数の桁数を $D$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| コピー・ムーブ構築、コピー・ムーブ代入 | $O(1)$ |
| デフォルト構築、`FastModint(v)` | $O(1)$ |
| `get_mod()`、`raw(v)`、`val()` | $O(1)$ |
| `+=`、`-=`、`*=`、`+`、`-`、`*` | $O(1)$ |
| 単項 `+`、単項 `-`、`++`、`--` | $O(1)$ |
| `==`、`!=` | $O(1)$ |
| `pow(n)` | $O(\log(n+1))$ |
| `inv()`、`/=`、`/` | $O(\log \mathrm{mod})$ |
| `operator>>`、`operator<<` | $O(D)$（ストリーム自身の処理を含む） |

## 空間計算量

オブジェクトの保存領域と、入出力以外の各関数の追加領域は $O(1)$ である。`pow`、`inv`、除算は反復で実装しており、スタックを消費しない。

## 注意点

- $\mathrm{mod} \ge 1$ でなければならない。`raw(v)` は正規化と検査を省略するため、$0 \le v \lt \mathrm{mod}$ は呼び出し側が保証する。
- `pow` の指数が負なら `std::runtime_error` を送出する。
- `inv` と除算は逆元の存在を要求する。法が素数のときの $0$、合成数のときの法と互いに素でない値では `std::runtime_error` を送出する。
- ストリームからの入力に失敗すると `std::runtime_error` を送出する。

---
title: Modint (剰余整数)
documentation_of: ../../../src/structure/modint/modint.hpp
---

法 `mod` を固定した剰余整数の型。

## 使い方

```cpp
using mint = Modint<998244353>;

mint a = 2;
mint b = 3;
mint c = (a + b) * b; // 15
mint d = c / b;       // 5
mint e = mint::raw(10);
```

## メンバ関数

- `Modint()`、`Modint(long long v)`
    - $v$ を $[0, \mathrm{mod})$ に正規化して構築する（デフォルトは $0$）。
- `static constexpr int get_mod()`
    - 法を返す。
- `static Modint raw(int v)`
    - 正規化せずに $v$ を内部値として構築する。
    - $0 \le v \lt \mathrm{mod}$ が必要である。
- `int val() const`
    - 内部値を返す。
- `Modint pow(long long n) const`
    - $n$ 乗を返す。
    - $n \lt 0$ の場合は例外を送出する。
- `Modint inv() const`
    - 乗法逆元を返す。
    - 法が素数のときは Fermat の小定理を、そうでないときは拡張 Euclid の互除法を使う。

## 演算

- `+`、`-`、`*`、`/`
- `+=`、`-=`、`*=`、`/=`
- 単項 `+`、`-`
- `++`、`--`
- `==`、`!=`
- `operator>>`、`operator<<`

## エイリアス

```cpp
using Modint998244353 = Modint<998244353>;
```

## 時間計算量

入出力については、整数の桁数を $D$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| コピー・ムーブ構築、コピー・ムーブ代入 | $O(1)$ |
| デフォルト構築、`Modint(v)` | $O(1)$ |
| `get_mod()`、`raw(v)`、`val()` | $O(1)$ |
| `+=`、`-=`、`*=`、`+`、`-`、`*` | $O(1)$ |
| 単項 `+`、単項 `-`、`++`、`--` | $O(1)$ |
| `==`、`!=` | $O(1)$ |
| `pow(n)` | $O(\log(n+1))$ |
| `inv()`、`/=`、`/` | $O(\log \mathrm{mod})$ |
| `operator>>`、`operator<<` | $O(D)$（ストリーム自身の処理を含む） |

## 空間計算量

各値は 32 bit の符号なし整数を 1 個保持する。各関数の追加領域は $O(1)$ である。

## 注意点

- テンプレート引数は $\mathrm{mod} \ge 1$ でなければならない。`raw(v)` の $0 \le v \lt \mathrm{mod}$ は検査しない前提である。
- `pow(n)` は非負の指数を要求し、負なら `std::runtime_error` を送出する。
- 法が素数のときの $0$ の `inv()`、合成数のときの法と互いに素でない値の `inv()` は `std::runtime_error` を送出する。
- `/` と `/=` は右辺の `inv()` を使うため、同じ逆元の存在条件を要求する。
- ストリームからの入力は符号付き 64 bit 整数として読み、入力に失敗すると `std::runtime_error` を送出する。

---
title: Modint
documentation_of: ../src/structure/modint/modint.hpp
---

固定 mod の整数型。

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

- `Modint(long long v = 0)`
    - `v` を mod で正規化して構築する。
- `static constexpr int get_mod()`
    - mod を返す。
- `static Modint raw(int v)`
    - 正規化せずに `v` を内部値として構築する。
    - `0 <= v < mod` が必要。
- `int val() const`
    - 内部値を返す。
- `Modint pow(long long n) const`
    - `n` 乗を返す。
    - `n < 0` の場合は例外。
- `Modint inv() const`
    - 逆元を返す。
    - mod が素数のときは Fermat、そうでないときは拡張 Euclid を使う。

## 演算

- `+`, `-`, `*`, `/`
- `+=`, `-=`, `*=`, `/=`
- 単項 `+`, `-`
- `++`, `--`
- `==`, `!=`
- `operator>>`, `operator<<`

## エイリアス

```cpp
using Modint998244353 = Modint<998244353>;
```

## 計算量

入出力については、整数の桁数を $D$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| コピー・move構築、コピー・move代入 | $O(1)$ |
| デフォルト構築、`Modint(v)` | $O(1)$ |
| `get_mod()`, `raw(v)`, `val()` | $O(1)$ |
| `+=`, `-=`, `*=`, `+`, `-`, `*` | $O(1)$ |
| 単項 `+`, 単項 `-`, `++`, `--` | $O(1)$ |
| `==`, `!=` | $O(1)$ |
| `pow(n)` | $O(\log(n+1))$ |
| `inv()`, `/=`, `/` | $O(\log mod)$ |
| `operator>>`, `operator<<` | $O(D)$（ストリーム自身の処理を含む） |

## 前提・例外

- template引数は`mod >= 1`。`raw(v)`は`0 <= v < mod`が未検査の前提。
- `pow(n)`は非負指数を要求し、負なら例外。
- 素数法では0の`inv()`、合成数法では値とmodが互いに素でない`inv()`が例外。
- `/`と`/=`は右辺の`inv()`を使うため、同じ逆元存在条件を要求する。
- stream入力は符号付き64bit整数を読み、入力失敗時は例外。
- 各値は1個の32bit符号なし整数を保持し、追加領域は$O(1)$。

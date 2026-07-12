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

- 加算・減算: $O(1)$
- 乗算: $O(1)$
- `pow`: $O(\log n)$
- `inv`: $O(\log mod)$

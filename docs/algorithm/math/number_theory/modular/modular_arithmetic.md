---
title: Modular Arithmetic (剰余演算) [primitive_root]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/modular_arithmetic.hpp
---

中国剰余定理と、素数を法とする原始根を提供する。

## 使い方

```cpp
auto result = math::chinese_remainder(r0, m0, r1, m1);
auto result = math::chinese_remainder(remainders, moduli);
auto generator = math::primitive_root(prime);
```

## API

### `ChineseRemainderResult`

`long long` のメンバ `remainder` と `modulus` を持つ。`remainder` は $[0, \mathrm{modulus})$ に正規化された最小の非負解、`modulus` は統合後の正の法で、すべての解は $\mathrm{remainder} + k \cdot \mathrm{modulus}$ と表される。メンバの参照は $O(1)$。

### `chinese_remainder(r0, m0, r1, m1)`

$x \equiv r_0 \pmod{m_0}$ かつ $x \equiv r_1 \pmod{m_1}$ を統合し、`std::optional<ChineseRemainderResult>` を返す。法は互いに素でなくてもよい。剰余は負でもよく、内部で正規化する。

- 解がなければ `std::nullopt` を返す。
- 法が正でない場合、または法の最小公倍数が `long long` に収まらない場合は `std::runtime_error` を送出する。

### `chinese_remainder(remainders, moduli)`

`std::vector<long long>` で与えた同じ添字の剰余と法を、先頭から順に統合する。長さが異なる場合は `std::runtime_error` を送出する。空列では `{remainder = 0, modulus = 1}` を返す。

### `primitive_root(prime)`

64 bit 素数 `prime` の最小の原始根を `u64` で返す。`prime == 2` では $1$ を返す。内部で Miller–Rabin による素数判定と Pollard–Rho による $\mathrm{prime}-1$ の素因数分解を行い、合成数と判定した場合は `std::runtime_error` を送出する。

## 時間計算量・空間計算量

$M_i$ を先頭 $i$ 個を統合した法、$F$ を $\mathrm{prime}-1$ の相異なる素因数の個数、$G$ を見つかった原始根とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 2 つの合同式の `chinese_remainder` | $O(\log \min(m_0, m_1))$ | $O(1)$ |
| $K$ 個の合同式の `chinese_remainder` | $O(\sum_{i=0}^{K-1} \log \min(M_i, m_i))$ | $O(1)$ |
| `primitive_root(prime)` | Pollard–Rho 素因数分解 + $O(GF \log \mathrm{prime})$ | 素因数分解に依存 |

## 注意点

中間値は `__int128` で計算するので、法が `long long` に収まる範囲では途中でオーバーフローしない。

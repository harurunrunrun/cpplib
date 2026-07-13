---
title: Modular Arithmetic
documentation_of: ../src/algorithm/math/modular_arithmetic.hpp
---

中国剰余定理と素数modの原始根。

## chinese_remainder

```cpp
auto result = math::chinese_remainder(r0, m0, r1, m1);
auto result = math::chinese_remainder(remainders, moduli);
```

解があれば `remainder` と正の `modulus` を返し、すべての解は
`remainder + k * modulus` で表される。法は互いに素でなくてもよい。
解がなければ `nullopt` を返す。統合後の法が `long long` に収まる必要がある。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| 2合同式の `chinese_remainder` | $O(\log\min(m_0,m_1))$ | $O(1)$ |
| $K$合同式の `chinese_remainder` | $O(\sum_{i=0}^{K-1}\log\min(M_i,m_i))$ | $O(1)$ |

$M_i$ は先頭 $i$ 個を統合した法とする。返り値のfield参照は $O(1)$。

## primitive_root

```cpp
auto generator = math::primitive_root(prime);
```

64 bit素数 `prime` の最小の原始根を返す。合成数は受け付けない。

`prime-1` の異なる素因数数を $F$、見つかった原始根を $G$ とする。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `primitive_root(prime)` | Pollard--Rho素因数分解 + $O(GF\log prime)$ | 素因数分解に依存 |

Miller--Rabinによる素数判定とPollard--Rhoによる素因数分解を内部で行う。

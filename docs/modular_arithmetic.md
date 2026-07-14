---
title: Modular Arithmetic (剰余演算) [primitive_root]
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

| 操作 | 時間計算量 | 空間計算量（追加領域） |
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

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `primitive_root(prime)` | Pollard--Rho素因数分解 + $O(GF\log prime)$ | 素因数分解に依存 |

Miller--Rabinによる素数判定とPollard--Rhoによる素因数分解を内部で行う。
## API詳細

### `ChineseRemainderResult`

`remainder` は $[0,modulus)$ に正規化された最小非負解、`modulus` は
統合後の正の法。field参照は $O(1)$。

### `chinese_remainder(r0, m0, r1, m1)`

$x\equiv r_0\pmod {m_0}$ かつ $x\equiv r_1\pmod {m_1}$ を統合する。
両方の法は正でなければならず、違反時は例外。解がなければ `nullopt`、
法の最小公倍数が `long long` を超える場合も例外を送出する。

### `chinese_remainder(remainders, moduli)`

同じ添字の剰余と法を左から統合する。長さが異なる場合は例外。
空列では `{remainder=0, modulus=1}` を返す。

### `primitive_root(prime)`

素数 `prime` の最小の原始根を返す。`prime == 2` では1。
Miller--Rabinで合成数と判定した場合は例外を送出する。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。

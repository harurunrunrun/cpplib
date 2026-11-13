---
title: XOR Basis (XOR基底) [ABC283_G]
documentation_of: ../../../src/structure/bit/xor_basis.hpp
---

符号なし整数を $\mathbb{F}_2$ 上のベクトルとみなした線形基底。基底は行簡約した形で保持する。

```cpp
template<class UInt, int BITS = std::numeric_limits<UInt>::digits>
struct XorBasis;
```

テンプレート引数 `BITS` は 1 以上かつ `UInt` のビット数以下とし、渡す値は下位 `BITS` ビットに収まる必要がある。

```cpp
XorBasis<unsigned long long> basis;
basis.insert(x);
basis.contains(x);
basis.maximize(seed);
basis.minimize(seed);
basis.kth_smallest(k);
basis.vectors();
basis.merge(other);
```

- `insert(x)`: `x` を基底に加える。階数が増えた場合だけ `true` を返す。
- `contains(x)`: `x` が基底の張る空間に含まれるかを返す。
- `maximize(seed = 0)`, `minimize(seed)`: `seed` と基底の XOR で作れる値の最大値・最小値を返す。
- `kth_smallest(k)`: 生成可能な相異なる値を昇順に並べたときの 0-indexed で `k` 番目の値を返す。階数が 64 なら、`unsigned long long` で表せるすべての `k` が有効である。
- `vectors()`: 基底ベクトルを主成分のビット位置の昇順に返す。
- `merge(other)`: `other` の基底ベクトルをすべて `insert` する。戻り値はない。
- `rank()`, `empty()`, `clear()`: 階数、空かどうか、全消去。

## 時間計算量

テンプレート引数 `BITS` を $B$、現在の階数を $R$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `XorBasis()` | $O(B)$ | $O(B)$ |
| `clear()` | $O(B)$ | $O(1)$ |
| `rank()`, `empty()` | $O(1)$ | $O(1)$ |
| `insert(value)` | $O(B)$ | $O(1)$ |
| `contains(value)` | $O(B)$ | $O(1)$ |
| `maximize(seed)`, `minimize(seed)` | $O(B)$ | $O(1)$ |
| `kth_smallest(k)` | $O(B)$ | $O(1)$ |
| `vectors()` | $O(B)$ | 出力 $O(R)$ |
| `merge(other)` | $O(B^2)$ | $O(1)$ |

## 注意点

- `UInt` は符号なし整数型で、`1 <= BITS <= std::numeric_limits<UInt>::digits` でなければならない（`static_assert`）。
- 入力値と `seed` は下位 `BITS` ビットに収まる必要があり、違反すると `std::runtime_error` を投げる。
- `kth_smallest(k)` は $k<2^R$ を要求し、範囲外なら `std::runtime_error` を投げる。

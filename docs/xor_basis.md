---
title: XOR Basis
documentation_of: ../src/structure/other/xor_basis.hpp
---

符号なし整数を $\mathbb{F}_2$ 上のbit vectorとみなす線形基底。
基底は行簡約された形で保持する。
template引数 `BITS` は1以上かつ整数型のbit数以下とし、渡す値は
下位 `BITS` bitに収まる必要がある。

```cpp
XorBasis<unsigned long long> basis;
basis.insert(x);
basis.contains(x);
basis.maximize(seed);
basis.minimize(seed);
basis.kth_smallest(k);
```

`kth_smallest(k)` は生成可能な相異なる値を昇順に並べた0-indexedの第 `k` 値を返す。
rankが64なら `unsigned long long` で表せるすべての `k` が有効である。
`vectors()` はpivot bitの昇順で基底vectorを返す。

## 計算量

template引数のbit数を $B$、現在のrankを $R$ とする。

| 操作 | 時間計算量 | 追加領域 |
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

## 前提・例外

- `UInt`は符号なし整数、`1 <= BITS <= numeric_limits<UInt>::digits`。
- 入力値とseedは下位`BITS` bitに収まる必要があり、違反時は例外。
- `kth_smallest(k)`は`k < 2^rank`を要求し、範囲外は例外。
- `insert`はrankが増えた場合だけ`true`、`merge`は戻り値を持たない。

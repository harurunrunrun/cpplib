---
title: Exact Integer In-Sphere (整数三次元外接球の正確判定)
documentation_of: ../src/algorithm/geometry/3d/exact_insphere.hpp
---

## API

```cpp
int exact_insphere(first, second, third, fourth, query)
```

`IntegerPoint3` の四面体の外接球について、頂点順に依存せず、球内なら1、球面上なら0、球外なら-1を返す。計算にはライブラリ内の `ExactInteger` を使い、外部の多倍長整数ライブラリには依存しない。

## API別の時間計算量・空間計算量

座標のbit長を $B$ とすると、定数個の $O(B)$〜$O(B^2)$ 規模の多倍長演算を行い、追加領域 $O(B)$。固定次元なので点数に関しては $O(1)$。

## 注意点

全 `int64_t` 座標域で正確。四面体が退化している場合は `std::invalid_argument`。判定値だけを返し、外接球中心や半径の構成は行わない。

---
title: Exact Integer In-Sphere (整数三次元外接球の正確判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/exact_insphere.hpp
---

## API

```cpp
int exact_insphere(first, second, third, fourth, query)
```

`IntegerPoint3` の四面体の外接球について、`query` が球の内部なら `1`、球面上なら `0`、外部なら `-1` を返す。結果は四面体の頂点順に依存しない。計算にはライブラリ内の `ExactInteger` を使い、外部の多倍長整数ライブラリには依存しない。

## API別の時間計算量・空間計算量

座標のビット長を $B$ とすると、$O(B)$ から $O(B^2)$ 規模の多倍長演算を定数回行い、追加領域は $O(B)$。次元が固定なので、点数に関しては $O(1)$ である。

## 注意点

`std::int64_t` の全座標範囲で正確である。四面体が退化している場合は `std::invalid_argument` を送出する。判定値だけを返し、外接球の中心や半径は構成しない。

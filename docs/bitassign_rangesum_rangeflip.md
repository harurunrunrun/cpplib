---
title: BitAssign RangeSum RangeFlip
documentation_of: ../src/structure/segtree/bitassign_rangesum_rangeflip.hpp
---

$0,1$ のみの列に対して、区間更新 と 区間和取得、区間反転を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- `Bit01WST(n)`, `Bit01WST(vector)`, `init`, `build`: $O(N)$
- `assign`, `flip`, `sum`, `get`: $O(\log N)$
- `size`: $O(1)$

# API

```cpp
Bit01WST(size_t n)
Bit01WST(const vector<int>& values)
void init(size_t n)
void build()
size_t size() const
void assign(size_t l, size_t r, bool value)
void flip(size_t l, size_t r)
uint64_t sum(size_t l, size_t r)
bool get(size_t k)
```

`assign/flip` は半開区間 `[l,r)` を代入・反転する。`sum` は1の個数、
`get` は1点を返す。vectorの非零要素は1。`init` は0列、`build` は集約値を再構築する。
範囲違反はassertion failure。各APIの計算量は上表の通り。

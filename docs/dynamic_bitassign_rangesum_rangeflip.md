---
title: Dynamic BitAssign RangeSum RangeFlip (動的ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大な $0,1$ のみの列に対して、区間更新 と 区間和取得、区間反転を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- default constructor, `DynamicBit01WST(n)`, `init`: $O(1)$
- `DynamicBit01WST(vector)`: $O(P\log N)$。$P$ は1の個数
- `assign`, `flip`, `sum`, `get`: $O(\log N)$
- `size`, `node_count`: $O(1)$

# API

```cpp
DynamicBit01WST()
DynamicBit01WST(size_t n)
DynamicBit01WST(const vector<int>& values)
void init(size_t n)
size_t size() const
size_t node_count() const
void assign(size_t l, size_t r, bool value)
void flip(size_t l, size_t r)
uint64_t sum(size_t l, size_t r)
bool get(size_t k)
```

default constructorは空列、`init` は0列、vectorの非零要素は1。`assign/flip` は
半開区間を代入・反転する。`sum/get` は1の個数・1点、`node_count` は疎node数を返す。
範囲違反はassertion failure、確保失敗は `bad_alloc`。各APIの計算量は上表の通り。

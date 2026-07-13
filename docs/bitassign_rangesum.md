---
title: BitAssign RangeSum (ビット代入・区間和)
documentation_of: ../src/structure/segtree/bitassign_rangesum.hpp
---

$0,1$ のみの列に対して、区間更新 と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- `Bit01WST(n)`, `Bit01WST(vector)`, `init`, `build`: $O(N)$
- `assign`, `sum`, `get`: $O(\log N)$
- `size`: $O(1)$

# API

```cpp
Bit01WST(size_t n)
Bit01WST(const vector<int>& values)
void init(size_t n)
void build()
size_t size() const
void assign(size_t l, size_t r, bool value)
uint64_t sum(size_t l, size_t r)
bool get(size_t k)
```

`init` は0列へ初期化し、vectorの非零要素は1とする。`build` は集約値を再構築する。
`assign` は半開区間 `[l,r)` を代入し、`sum` は1の個数、`get` は1点を返す。
`0 <= l <= r <= size()`、`k < size()` が前提で、違反はassertion failure。
各APIの計算量は上表の通り。

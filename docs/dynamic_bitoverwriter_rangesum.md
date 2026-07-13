---
title: Dynamic BitOverwrite RangeSum (動的ビット上書き・区間和)
documentation_of: ../src/structure/segtree/dynamic_bitoverwrite_rangesum.hpp
---

巨大な $0,1$ のみの列に対して、区間を $1$ で上書き と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- default constructor, `DynamicWideSetSum(n)`, `init`: $O(1)$
- `set`, `sum`: $O(B\log_B(N/64+1))$
- `all_sum`: $O(1)$

`set`が新しく確保するノード数は $O(\log_B(N/64+1))$。

# API

```cpp
DynamicWideSetSum()
DynamicWideSetSum(size_t n)
void init(size_t n)
void set(size_t l, size_t r)
uint64_t sum(size_t l, size_t r)
uint64_t all_sum() const
```

default constructorは空列、`init` は0列。`set` は半開区間 `[l,r)` を1で上書きし、
`sum/all_sum` は区間・全体の1の個数を返す。範囲違反はassertion failure、
確保失敗は `bad_alloc`。各APIの計算量は上表の通り。

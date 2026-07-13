---
title: BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/bitoverwrite_rangesum.hpp
---

$0,1$ のみの列に対して、区間を $1$ で上書き と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- `WideSetSum(n)`, `init`: $O(N/64)$
- `set`, `sum`: $O(B\log_B(N/64+1))$
- `all_sum`: $O(1)$

# API

```cpp
WideSetSum(size_t n)
void init(size_t n)
void set(size_t l, size_t r)
uint64_t sum(size_t l, size_t r)
uint64_t all_sum() const
```

`init` は0列へ初期化する。`set` は半開区間 `[l,r)` を1で上書きし、
`sum` と `all_sum` は区間・全体の1の個数を返す。範囲違反はassertion failure。
各APIの計算量は上表の通り。

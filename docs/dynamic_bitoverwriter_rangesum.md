---
title: Dynamic BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/dynamic_bitoverwrite_rangesum.hpp
---

巨大な $0,1$ のみの列に対して、区間を $1$ で上書き と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- default constructor, `DynamicWideSetSum(n)`, `init`: $O(1)$
- `set`, `sum`: $O(B\log_B(N/64+1))$
- `all_sum`: $O(1)$

`set`が新しく確保するノード数は $O(\log_B(N/64+1))$。

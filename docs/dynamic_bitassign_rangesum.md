---
title: Dynamic BitAssign RangeSum (動的ビット代入・区間和)
documentation_of: ../src/structure/segtree/dynamic_bitassign_rangesum.hpp
---

巨大な $0,1$ のみの列に対して、区間更新 と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 時間計算量

- default constructor, `DynamicBit01WSTNoFlip(n)`, `init`: $O(1)$
- `DynamicBit01WSTNoFlip(vector)`: $O(P\log N)$。$P$ は1の個数
- `assign`, `sum`, `get`: $O(\log N)$
- `size`, `node_count`: $O(1)$

# API

```cpp
DynamicBit01WSTNoFlip()
DynamicBit01WSTNoFlip(size_t n)
DynamicBit01WSTNoFlip(const vector<int>& values)
void init(size_t n)
size_t size() const
size_t node_count() const
void assign(size_t l, size_t r, bool value)
uint64_t sum(size_t l, size_t r)
bool get(size_t k)
```

default constructorは空列、`init` は0列を作り、vectorの非零要素は1とする。
`assign` は半開区間 `[l,r)`、`sum` は1の個数、`get` は1点、
`node_count` は確保済み疎node数を返す。範囲違反はassertion failure、
確保失敗は `bad_alloc`。各APIの計算量は上表の通り。

## 注意点

indexは0-indexed、区間は `[l,r)`。範囲違反はassertion failure、node確保失敗は
`bad_alloc`。vectorの非零値は1として構築する。保存領域は生成node数を $K$ として
$O(K)$、更新1回の追加nodeは $O(\log N)$、queryの追加空間計算量は再帰stack
$O(\log N)$。

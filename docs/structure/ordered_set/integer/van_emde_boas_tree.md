---
title: Sparse van Emde Boas Tree (疎van Emde Boas木)
documentation_of: ../../../../src/structure/ordered_set/integer/van_emde_boas_tree.hpp
---

`[0,2^BITS)` のinteger集合を、空clusterを生成しないsparse van Emde Boas木で管理する。

## API

~~~cpp
VanEmdeBoasTree<BITS>()
bool empty() const
size_t size() const
bool contains(uint64_t key) const
bool insert(uint64_t key)
bool erase(uint64_t key)
uint64_t minimum() const
uint64_t maximum() const
optional<uint64_t> successor(uint64_t key) const
optional<uint64_t> predecessor(uint64_t key) const
~~~

`successor` / `predecessor` はstrictな後続 / 先行要素を返す。

## 時間計算量

hash table操作をexpected $O(1)$ として、検索・更新はexpected
$O(\log BITS)=O(\log\log U)$。`empty`, `size` は $O(1)$。
空間計算量はexpected $O(N\log BITS)$。

## 注意点

`1 <= BITS <= 63`。宇宙外keyは `out_of_range`。空集合のmin/maxも
`out_of_range`。

---
title: Y-fast Trie (Y-fastトライ)
documentation_of: ../../../../src/structure/ordered_set/integer/y_fast_trie.hpp
---

bounded integer集合に対してpredecessor/successorを提供する。

## API

~~~cpp
YFastTrie<BITS>()
bool empty() const
size_t size() const
bool contains(uint64_t key) const
bool insert(uint64_t key)
bool erase(uint64_t key)
uint64_t minimum() const
uint64_t maximum() const
optional<uint64_t> predecessor(uint64_t key) const
optional<uint64_t> successor(uint64_t key) const
optional<uint64_t> lower_bound(uint64_t key) const
~~~

## 時間計算量

hash table操作をexpected $O(1)$ として全検索・更新はexpected
$O(\log BITS)=O(\log\log U)$。`empty`, `size` は $O(1)$。

## 注意点

`1 <= BITS <= 63`。successor/predecessorはstrict。宇宙外keyと空集合の
min/maxは `out_of_range`。

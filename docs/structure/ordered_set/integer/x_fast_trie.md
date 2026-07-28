---
title: X-fast Trie (X-fastトライ)
documentation_of: ../../../../src/structure/ordered_set/integer/x_fast_trie.hpp
---

各depthのprefixをhash tableに置き、leafをsorted linked listで結ぶinteger集合。

## API

~~~cpp
XFastTrie<UInt, BITS>()
bool empty() const
size_t size() const
bool contains(UInt key) const
bool insert(UInt key)
bool erase(UInt key)
optional<UInt> lower_bound(UInt key) const
optional<UInt> predecessor(UInt key) const
optional<UInt> successor(UInt key) const
UInt minimum() const
UInt maximum() const
~~~

`predecessor` / `successor` はstrict、`lower_bound` はkey以上を返す。

## 時間計算量

hash table操作をexpected $O(1)$ とする。

- `contains`: expected $O(1)$
- `lower_bound`, `predecessor`, `successor`: expected $O(\log BITS)$
- `insert`, `erase`: expected $O(BITS)$
- `minimum`, `maximum`, `empty`, `size`: $O(1)$

空間計算量はexpected $O(N\cdot BITS)$。

## 注意点

`UInt` はunsigned、keyは `[0,2^BITS)`。宇宙外keyは `out_of_range`。

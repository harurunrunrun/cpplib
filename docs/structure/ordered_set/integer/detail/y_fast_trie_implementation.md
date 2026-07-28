---
title: Y-fast Trie Implementation (Y-fastトライ内部実装)
documentation_of: ../../../../../src/structure/ordered_set/integer/detail/y_fast_trie_implementation.hpp
---

Y-fast trieの内部部品。各bucketを `std::set`、最大keyのrepresentative集合を
X-fast trieで管理する。通常は `y_fast_trie.hpp` から利用する。

## API

~~~cpp
YFastTrieImplementation<UInt, BITS>()
bool empty() const
size_t size() const
bool contains(UInt key) const
bool insert(UInt key)
bool erase(UInt key)
UInt minimum() const
UInt maximum() const
optional<UInt> lower_bound(UInt key) const
optional<UInt> predecessor(UInt key) const
optional<UInt> successor(UInt key) const
~~~

## 時間計算量

hash table操作をexpected $O(1)$ とする。検索はexpected
$O(\log BITS)$。bucketのsplit/mergeを均すと更新もamortized expected
$O(\log BITS)$。空間計算量はexpected $O(N)$。

## 注意点

内部APIであり互換性は保証しない。`UInt` はunsignedでkeyは
`[0,2^BITS)`。

---
title: Cuckoo Hash Table (Cuckooハッシュ表)
documentation_of: ../../../src/structure/hash/cuckoo_hash_table.hpp
---

2個の候補slot間でentryを追い出して配置するhash map。与えられたhash関数が
異なるkeyを識別できない場合にも停止性と正しさを保つため、有限回の再構築後は
線形探索のfallbackへ移る。

## API

~~~cpp
CuckooHashTable<Key, Value, Hash, Equal>()
bool empty() const
size_t size() const
Value* find(const Key& key)
const Value* find(const Key& key) const
bool contains(const Key& key) const
bool insert_or_assign(const Key& key, const Value& value)
bool erase(const Key& key)
Value& at(const Key& key)
~~~

## 時間計算量

通常のcuckoo表では `find`, `contains`, `erase`, `at` は $O(1)$、追加は
expected amortized $O(1)$、rehash一回は $O(N)$。hash値が一致する異なるkeyを
分離できずfallbackへ移った場合、各操作はworst-case $O(N)$。空間は $O(N)$。

## 注意点

load factorを0.5以下に保つ。key/valueはcopy可能であること。存在しないkeyの
`at` は `out_of_range`。再構築回数には上限があり、退化したhash関数でも
無限にrehashしない。

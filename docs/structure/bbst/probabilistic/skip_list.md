---
title: Skip List (スキップリスト)
documentation_of: ../../../../src/structure/bbst/probabilistic/skip_list.hpp
---

random heightのforward linkを持つordered map。

## API

~~~cpp
SkipList<Key, Value, MAX_LEVEL, Compare>()
bool empty() const
size_t size() const
Value* find(const Key& key)
const Value* find(const Key& key) const
bool insert_or_assign(Key key, Value value)
bool erase(const Key& key)
optional<pair<Key, Value>> lower_bound(const Key& key) const
~~~

## 時間計算量

`find`, `insert_or_assign`, `erase`, `lower_bound` はexpected
$O(\log(N+1))$、worst-case $O(N)$。`empty`, `size` は $O(1)$、空間は
expected $O(N)$。

## 注意点

copy不可。`MAX_LEVEL` が要素数のbit lengthより十分大きいこと。

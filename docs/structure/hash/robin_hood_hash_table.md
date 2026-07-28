---
title: Robin Hood Hash Table (Robin Hoodハッシュ表)
documentation_of: ../../../src/structure/hash/robin_hood_hash_table.hpp
---

probe distanceが大きいentryを優先して配置するopen-addressing map。

## API

~~~cpp
RobinHoodHashTable<Key, Value, Hash, Equal>()
RobinHoodHashTable(size_t capacity)
bool empty() const
size_t size() const
bool insert_or_assign(Key key, Value value)
Value* find(const Key& key)
const Value* find(const Key& key) const
bool contains(const Key& key) const
Value& at(const Key& key)
bool erase(const Key& key)
~~~

## 時間計算量

良いhashを仮定して検索・追加・削除はexpected amortized $O(1)$。
rehash一回は $O(N)$。worst-caseは $O(N)$、空間は $O(N)$。

## 注意点

最大load factorは0.8。存在しないkeyの `at` は `out_of_range`。

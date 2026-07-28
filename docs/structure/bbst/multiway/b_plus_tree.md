---
title: B+ Tree (B+木)
documentation_of: ../../../../src/structure/bbst/multiway/b_plus_tree.hpp
---

valueをleafだけに持ち、leafをlinked listで結ぶB+ tree map。

## API

~~~cpp
BPlusTree<Key, Value, FANOUT, Compare>()
BPlusTree<Key, Value, FANOUT, Compare>(Compare compare)
bool empty() const
size_t size() const
bool insert_or_assign(const Key& key, const Value& value)
optional<Value> find(const Key& key) const
bool erase(const Key& key)
vector<pair<Key, Value>> range(const Key& lower, const Key& upper) const
~~~

`range` は半開区間 `[lower,upper)` をkey昇順で返す。

## 時間計算量

`B = FANOUT` とする。`find`, `insert_or_assign`, `erase` は
$O(B\log_B(N+1))$、`range` は $O(B\log_B(N+1)+K)$。空間は $O(N)$。

## 注意点

`erase` performs sibling borrowing, node merging, separator refresh, and root contraction.

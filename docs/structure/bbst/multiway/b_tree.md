---
title: B-Tree (B木)
documentation_of: ../../../../src/structure/bbst/multiway/b_tree.hpp
---

1 nodeに複数keyを持つminimum degree `MIN_DEGREE` のB-tree ordered set。

## API

~~~cpp
BTree<Key, MIN_DEGREE, Compare>()
bool empty() const
size_t size() const
bool contains(const Key& key) const
bool insert(const Key& key)
bool erase(const Key& key)
optional<Key> lower_bound(const Key& key) const
~~~

## 時間計算量

`B = MIN_DEGREE`, key比較・moveを $O(1)$ とする。検索・更新は
$O(B\log_B(N+1))$、`empty`, `size` は $O(1)$。空間は $O(N)$。

## 注意点

`MIN_DEGREE >= 2`。同値keyを重複保持しない。

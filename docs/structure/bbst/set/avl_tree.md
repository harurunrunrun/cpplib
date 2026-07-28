---
title: AVL Tree (AVL木)
documentation_of: ../../../../src/structure/bbst/set/avl_tree.hpp
---

各nodeの左右height差を高々1に保つordered set。subtree sizeも保持する。

## API

~~~cpp
AvlTree<Key, Compare>()
bool empty() const
size_t size() const
bool contains(const Key& key) const
bool insert(const Key& key)
bool erase(const Key& key)
optional<Key> lower_bound(const Key& key) const
optional<Key> kth(size_t index) const
~~~

`kth` は0-indexed sorted order。

## 時間計算量

`empty`, `size` は $O(1)$、他の各操作は $O(\log(N+1))$。空間は $O(N)$。

## 注意点

同値keyを重複保持しない。`kth` の範囲外は `nullopt`。

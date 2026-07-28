---
title: Scapegoat Tree (スケープゴート木)
documentation_of: ../../../../src/structure/bbst/set/scapegoat_tree.hpp
---

subtreeの偏りが閾値を超えた箇所を一括再構築するordered set。

## API

~~~cpp
ScapegoatTree<Key, Compare>()
bool empty() const
size_t size() const
bool contains(const Key& key) const
bool insert(const Key& key)
bool erase(const Key& key)
optional<Key> lower_bound(const Key& key) const
~~~

## 時間計算量

検索はworst-case $O(\log(N+1))$、追加・削除はamortized
$O(\log(N+1))$。一度の再構築は対象subtree sizeに線形。空間は $O(N)$。

## 注意点

同値keyを重複保持しない。再構築時にnodeのaddressは安定しない。

---
title: AVL Tree (AVL木)
documentation_of: ../../../../src/structure/bbst/set/avl_tree.hpp
---

各ノードの左右の部分木の高さの差を高々 1 に保つ AVL 木による順序付き集合。部分木の大きさも保持する。

## API

~~~cpp
template<class Key, class Compare = std::less<Key>>
class AvlTree;

AvlTree()
explicit AvlTree(Compare compare)
bool empty() const
std::size_t size() const
bool contains(const Key& key) const
bool insert(const Key& key)
bool erase(const Key& key)
std::optional<Key> lower_bound(const Key& key) const
std::optional<Key> kth(std::size_t index) const
~~~

- `insert`: キーが未登録なら追加して `true`、登録済みなら `false` を返す。
- `erase`: キーを削除し、存在したかどうかを返す。
- `lower_bound`: `key` 以上の最小のキーを返す。なければ `std::nullopt`。
- `kth`: 昇順で 0-indexed の `index` 番目のキーを返す。

## 時間計算量

要素数を $N$ とする。

- `empty`, `size`: $O(1)$
- その他の各操作: $O(\log(N+1))$
- 空間計算量: $O(N)$

## 注意点

- 同値なキーを重複して保持しない。
- `kth` の添字が範囲外なら `std::nullopt` を返す。

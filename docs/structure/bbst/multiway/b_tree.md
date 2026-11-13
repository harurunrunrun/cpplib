---
title: B-Tree (B木)
documentation_of: ../../../../src/structure/bbst/multiway/b_tree.hpp
---

1 つのノードに複数のキーを持つ、最小次数 `MIN_DEGREE` の B 木による順序付き集合。

## API

~~~cpp
template<class Key, std::size_t MIN_DEGREE = 16, class Compare = std::less<Key>>
class BTree;

BTree()
explicit BTree(Compare compare)
bool empty() const
std::size_t size() const
bool contains(const Key& key) const
bool insert(const Key& key)
bool erase(const Key& key)
std::optional<Key> lower_bound(const Key& key) const
~~~

- `insert`: キーが未登録なら追加して `true`、登録済みなら `false` を返す。
- `erase`: キーを削除し、存在したかどうかを返す。
- `lower_bound`: `key` 以上の最小のキーを返す。なければ `std::nullopt`。

## 時間計算量

$B=\texttt{MIN_DEGREE}$、要素数を $N$ とし、キーの比較・ムーブを $O(1)$ とする。

- `contains`, `insert`, `erase`, `lower_bound`: $O(B\log_B(N+1))$（各ノードでキーを線形探索する）
- `empty`, `size`: $O(1)$
- 空間計算量: $O(N)$

## 注意点

- `MIN_DEGREE` は 2 以上でなければならない（`static_assert`）。
- 同値なキーを重複して保持しない。

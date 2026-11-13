---
title: Scapegoat Tree (スケープゴート木)
documentation_of: ../../../../src/structure/bbst/set/scapegoat_tree.hpp
---

部分木の偏りが閾値を超えた箇所をまとめて再構築する、スケープゴート木による順序付き集合。挿入・削除の後、根までの経路上で大きい方の子の部分木が全体の $3/4$ を超えたノードの部分木を完全平衡に作り直す。

## API

~~~cpp
template<class Key, class Compare = std::less<Key>>
class ScapegoatTree;

ScapegoatTree()
explicit ScapegoatTree(Compare compare)
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

要素数を $N$ とする。

- `contains`, `lower_bound`: 最悪 $O(\log(N+1))$
- `insert`, `erase`: 償却 $O(\log(N+1))$。1 回の再構築は対象の部分木の大きさに比例する
- `empty`, `size`: $O(1)$
- 空間計算量: $O(N)$

## 注意点

- 同値なキーを重複して保持しない。
- 再構築はノードを付け替えるだけで、キーのコピーやノードの再確保は行わない。ただし再構築用に部分木の大きさ分の一時配列（$O(N)$）を確保し、再帰の深さは木の高さ $O(\log(N+1))$ である。

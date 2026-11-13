---
title: Standard B+ Tree Implementation (標準B+木内部実装)
documentation_of: ../../../../../src/structure/bbst/multiway/detail/standard_b_plus_tree.hpp
---

葉の連結リスト、内部ノードの区切りキー、削除時の借用・併合、根の縮約を備えた標準的な B+ 木の内部実装である。各内部ノードは最大 `FANOUT` 個の子を持つ。公開名は `BPlusTree`（`b_plus_tree.hpp`）。

## API

~~~cpp
template<class Key, class Value, std::size_t FANOUT = 32, class Compare = std::less<Key>>
class StandardBPlusTree;

StandardBPlusTree()
explicit StandardBPlusTree(Compare compare)
bool empty() const
std::size_t size() const
bool insert_or_assign(const Key& key, const Value& value)
std::optional<Value> find(const Key& key) const
bool erase(const Key& key)
std::vector<std::pair<Key, Value>> range(const Key& lower, const Key& upper) const
~~~

`insert_or_assign` は新しいキーを追加したかどうかを返す。`range` は半開区間 $[\mathrm{lower},\mathrm{upper})$ の要素をキーの昇順で返す。

## 時間計算量

$B=\texttt{FANOUT}$、要素数を $N$、`range` の出力数を $K$ とし、キー・値の操作を $O(1)$ とする。

- `empty`, `size`: $O(1)$
- `find`: $O(\log(N+1))$
- `insert_or_assign`, `erase`: $O(B\log_B(N+1))$
- `range`: $O(\log(N+1)+K)$
- 空間計算量: $O(N)$

## 注意点

- `FANOUT` は 4 以上でなければならない（`static_assert`）。
- 同じキーの追加は値を上書きする。
- 内部 API なので互換性は保証しない。

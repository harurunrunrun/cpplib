---
title: Standard B+ Tree Implementation (標準B+木内部実装)
documentation_of: ../../../../../src/structure/bbst/multiway/detail/standard_b_plus_tree.hpp
---

leaf linked list、internal separator、削除時のborrow/merge、root縮約を備える
標準B+ treeの内部実装。各nodeは最大 `FANOUT` childrenを持つ。

## API

~~~cpp
StandardBPlusTree<Key, Value, FANOUT, Compare>()
StandardBPlusTree(Compare compare)
bool empty() const
size_t size() const
bool insert_or_assign(const Key& key, const Value& value)
optional<Value> find(const Key& key) const
bool erase(const Key& key)
vector<pair<Key, Value>> range(const Key& lower, const Key& upper) const
~~~

`range` は半開区間 `[lower,upper)` をkey昇順で返す。

## 時間計算量

`B = FANOUT`、key/value操作を $O(1)$ とする。

- `empty`, `size`: $O(1)$
- `find`, `insert_or_assign`, `erase`: $O(B\log_B(N+1))$
- `range`: $O(B\log_B(N+1)+K)$

空間計算量は $O(N)$。

## 注意点

`FANOUT >= 4`。同じkeyの追加はvalueを上書きする。内部APIの互換性は
保証しない。

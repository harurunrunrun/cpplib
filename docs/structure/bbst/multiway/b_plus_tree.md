---
title: B+ Tree (B+木)
documentation_of: ../../../../src/structure/bbst/multiway/b_plus_tree.hpp
---

値を葉だけに持ち、葉を連結リストで結んだ B+ 木による順序付き連想配列。`BPlusTree` は内部実装 `StandardBPlusTree` の別名である。

## API

~~~cpp
template<class Key, class Value, std::size_t FANOUT = 32, class Compare = std::less<Key>>
using BPlusTree = StandardBPlusTree<Key, Value, FANOUT, Compare>;

BPlusTree()
explicit BPlusTree(Compare compare)
bool empty() const
std::size_t size() const
bool insert_or_assign(const Key& key, const Value& value)
std::optional<Value> find(const Key& key) const
bool erase(const Key& key)
std::vector<std::pair<Key, Value>> range(const Key& lower, const Key& upper) const
~~~

- `insert_or_assign`: キーが未登録なら追加して `true`、登録済みなら値を上書きして `false` を返す。
- `find`: キーに対応する値を返す。なければ `std::nullopt`。
- `erase`: キーを削除し、存在したかどうかを返す。
- `range`: 半開区間 $[\mathrm{lower},\mathrm{upper})$ の要素をキーの昇順で返す。

## 時間計算量

$B=\texttt{FANOUT}$、要素数を $N$、`range` の出力数を $K$ とする。

- `empty`, `size`: $O(1)$
- `find`: $O(\log(N+1))$（各ノードで二分探索）
- `insert_or_assign`, `erase`: $O(B\log_B(N+1))$
- `range`: $O(\log(N+1)+K)$
- 空間計算量: $O(N)$

## 注意点

- `FANOUT` は 4 以上でなければならない（`static_assert`）。
- `erase` は兄弟ノードからの借用、ノードの併合、区切りキーの更新、根の縮約を行う。
- コピーはできないが、ムーブはできる。

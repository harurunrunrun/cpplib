---
title: Skip List (スキップリスト)
documentation_of: ../../../../src/structure/bbst/probabilistic/skip_list.hpp
---

ランダムな高さの前向きリンクを持つスキップリストによる順序付き連想配列。

## API

~~~cpp
template<class Key, class Value, std::size_t MAX_LEVEL = 32, class Compare = std::less<Key>>
class SkipList;

SkipList()
explicit SkipList(Compare compare)
bool empty() const
std::size_t size() const
Value* find(const Key& key)
const Value* find(const Key& key) const
bool insert_or_assign(Key key, Value value)
bool erase(const Key& key)
std::optional<std::pair<Key, Value>> lower_bound(const Key& key) const
~~~

- `find`: キーに対応する値へのポインタを返す。なければ `nullptr`。
- `insert_or_assign`: キーが未登録なら追加して `true`、登録済みなら値を上書きして `false` を返す。
- `erase`: キーを削除し、存在したかどうかを返す。
- `lower_bound`: `key` 以上の最小のキーを持つ要素のコピーを返す。なければ `std::nullopt`。

## 時間計算量

要素数を $N$ とする。

- `find`, `insert_or_assign`, `erase`, `lower_bound`: 期待 $O(\log(N+1))$、最悪 $O(N)$
- `empty`, `size`: $O(1)$
- 空間計算量: 期待 $O(N + \texttt{MAX_LEVEL})$（各ノードは自身の高さ分だけのリンク配列を持ち、高さの期待値は 2。先頭の番兵だけが長さ `MAX_LEVEL` のリンク配列を持つ）

## 注意点

- コピーもムーブもできない。
- 各ノードの高さは確率 $1/2$ で 1 ずつ伸び、最大 `MAX_LEVEL` である。`MAX_LEVEL` は要素数のビット長より十分大きくする。
- 乱数は固定シードの xorshift なので、同じ操作列に対して同じ構造になる。

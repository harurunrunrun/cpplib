---
title: X-fast Trie (X-fastトライ)
documentation_of: ../../../../src/structure/ordered_set/integer/x_fast_trie.hpp
---

各深さの接頭辞（上位ビット）をハッシュテーブルに格納し、葉をソート済みの連結リストでつないだ整数集合である。

## API

~~~cpp
XFastTrie<UInt, BITS>()
bool empty() const
size_t size() const
bool contains(UInt key) const
bool insert(UInt key)
bool erase(UInt key)
optional<UInt> lower_bound(UInt key) const
optional<UInt> predecessor(UInt key) const
optional<UInt> successor(UInt key) const
UInt minimum() const
UInt maximum() const
~~~

`UInt` のデフォルトは `std::uint64_t`、`BITS` のデフォルトは `UInt` のビット幅である。`insert` / `erase` は集合が変化したかを返す。`predecessor` / `successor` は `key` より真に小さい最大の要素 / 真に大きい最小の要素を、`lower_bound` は `key` 以上の最小の要素を返し、存在しなければ `std::nullopt` を返す。

## 時間計算量

ハッシュテーブルの操作を期待 $O(1)$ とし、$B$ = `BITS`、要素数を $N$ とする。

- `contains`: 期待 $O(1)$
- `lower_bound`, `predecessor`, `successor`: 期待 $O(\log B)$
- `insert`, `erase`: 期待 $O(B)$
- `minimum`, `maximum`, `empty`, `size`: $O(1)$

空間計算量は期待 $O(NB)$。

## 注意点

- `UInt` は符号なし整数型、`1 <= BITS <= ` `UInt` のビット幅であることを `static_assert` で要求する。
- キーは $[0, 2^{B})$ にあること。宇宙外のキーを渡すと `std::out_of_range` を送出する。
- 空集合に対する `minimum` / `maximum` は `std::out_of_range` を送出する。

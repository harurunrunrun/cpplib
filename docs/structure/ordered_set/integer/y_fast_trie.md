---
title: Y-fast Trie (Y-fastトライ)
documentation_of: ../../../../src/structure/ordered_set/integer/y_fast_trie.hpp
---

$[0, 2^{B})$（$B$ = `BITS`）上の整数集合に対して、predecessor / successor クエリを提供する。`YFastTrieImplementation<std::uint64_t, BITS>` をそのまま公開したものである。

## API

~~~cpp
YFastTrie<BITS>()
bool empty() const
size_t size() const
bool contains(uint64_t key) const
bool insert(uint64_t key)
bool erase(uint64_t key)
uint64_t minimum() const
uint64_t maximum() const
optional<uint64_t> predecessor(uint64_t key) const
optional<uint64_t> successor(uint64_t key) const
optional<uint64_t> lower_bound(uint64_t key) const
~~~

## 時間計算量

ハッシュテーブルの操作を期待 $O(1)$ とし、$U = 2^{B}$ とする。

- 検索（`contains`, `minimum`, `maximum`, `predecessor`, `successor`, `lower_bound`）: 期待 $O(\log B) = O(\log \log U)$
- `insert`, `erase`: ならし期待 $O(\log B) = O(\log \log U)$
- `empty`, `size`: $O(1)$

空間計算量は期待 $O(N)$（$N$ は要素数）。

## 注意点

- `1 <= BITS <= 64` を要求する（内部の `XFastTrie` の `static_assert` による）。
- `insert` / `erase` は集合が変化したかを返す。`successor` / `predecessor` は `key` より真に大きい / 真に小さい要素を返し、`lower_bound` は `key` 以上の最小の要素を返す。存在しなければ `std::nullopt` を返す。
- 宇宙外のキーを渡すと `std::out_of_range` を送出する。空集合に対する `minimum` / `maximum` も `std::out_of_range` を送出する。

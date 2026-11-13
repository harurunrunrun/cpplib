---
title: Y-fast Trie Implementation (Y-fastトライ内部実装)
documentation_of: ../../../../../src/structure/ordered_set/integer/detail/y_fast_trie_implementation.hpp
---

Y-fast trie の内部実装である。要素を大きさ $\Theta(B)$（$B$ = `BITS`）程度のバケットに分けて各バケットを `std::set` で持ち、各バケットの最大キー（代表元）の集合を X-fast trie で管理する。通常は `y_fast_trie.hpp` から利用する。

## API

~~~cpp
YFastTrieImplementation<UInt, BITS>()
bool empty() const
size_t size() const
bool contains(UInt key) const
bool insert(UInt key)
bool erase(UInt key)
UInt minimum() const
UInt maximum() const
optional<UInt> lower_bound(UInt key) const
optional<UInt> predecessor(UInt key) const
optional<UInt> successor(UInt key) const
~~~

## 時間計算量

ハッシュテーブルの操作を期待 $O(1)$ とする。検索は期待 $O(\log B)$。バケットの分割・併合のコストをならすと、更新もならし期待 $O(\log B)$ である。空間計算量は期待 $O(N)$（$N$ は要素数）。

## 注意点

内部 API であり、互換性は保証しない。`UInt` は符号なし整数型で、キーは $[0, 2^{B})$ にあること。宇宙外のキーは `std::out_of_range` を送出し、空集合に対する `minimum` / `maximum` も `std::out_of_range` を送出する。

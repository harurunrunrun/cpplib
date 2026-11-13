---
title: Sparse van Emde Boas Tree (疎van Emde Boas木)
documentation_of: ../../../../src/structure/ordered_set/integer/van_emde_boas_tree.hpp
---

$[0, 2^{B})$（$B$ = `BITS`）上の整数集合を、空のクラスタを生成しない疎な van Emde Boas 木で管理する。

## API

~~~cpp
VanEmdeBoasTree<BITS>()
bool empty() const
size_t size() const
bool contains(uint64_t key) const
bool insert(uint64_t key)
bool erase(uint64_t key)
uint64_t minimum() const
uint64_t maximum() const
optional<uint64_t> successor(uint64_t key) const
optional<uint64_t> predecessor(uint64_t key) const
~~~

`insert` / `erase` は集合が変化したかを返す。`successor` / `predecessor` は `key` より真に大きい最小の要素 / 真に小さい最大の要素を返し、存在しなければ `std::nullopt` を返す。

## 時間計算量

ハッシュテーブルの操作を期待 $O(1)$ とし、$U = 2^{B}$、要素数を $N$ とする。

- `contains`, `insert`, `erase`, `minimum`, `maximum`, `successor`, `predecessor`: 期待 $O(\log B) = O(\log \log U)$
- `empty`, `size`: $O(1)$

空間計算量は期待 $O(N \log B)$。

## 注意点

- `1 <= BITS <= 63` を `static_assert` で要求する。
- 宇宙外のキーを渡すと `std::out_of_range` を送出する。
- 空集合に対する `minimum` / `maximum` も `std::out_of_range` を送出する。

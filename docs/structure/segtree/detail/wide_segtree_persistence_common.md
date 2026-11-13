---
title: Wide Segment Tree Persistence Common (広域セグメント木永続化共通部品)
documentation_of: ../../../../src/structure/segtree/detail/wide_segtree_persistence_common.hpp
---

広い添字範囲を扱う永続・部分永続・ロールバック可能な区間木が共有する、加算のモノイドである。

## API

- `wide_segtree_persistence_internal::sum_op(left, right)`: 2 つの `long long` の和を返す。
- `wide_segtree_persistence_internal::sum_identity()`: 加算の単位元 `0` を返す。
- `wide_segtree_persistence_internal::sum_monoid`: 上記の 2 関数からなる `Monoid` 定数である。

## 時間計算量

各関数とモノイドの演算は、時間・追加空間計算量ともに $O(1)$ である。

## 注意点

- `PersistentWidesegtree` 系の内部部品であり、通常は直接使わない。
- `long long` の加算がオーバーフローしないことは呼び出し側が保証する。

---
title: Wide Segment Tree Persistence Common (広域セグメント木永続化共通部品)
documentation_of: ../src/structure/segtree/wide_segtree_persistence_common.hpp
---

広い添字範囲を扱う永続・部分永続・rollback区間木が共有する加算モノイド。

## API

- `wide_segtree_persistence_internal::sum_op(left,right)`：2つの `long long` を加える。
- `wide_segtree_persistence_internal::sum_identity()`：加算単位元 `0` を返す。
- `wide_segtree_persistence_internal::sum_monoid`：上記2関数からなる `Monoid` 定数。

## 時間計算量

各関数とモノイド演算は時間・追加空間計算量ともに $O(1)$。

## 注意点

- `PersistentWidesegtree` 系の内部部品であり、通常は直接使用しない。
- `long long` の加算が表現範囲を超えないことを呼出側が保証する。

---
title: Persistent Range Affine Sum with Copy (区間コピー付き永続アフィン作用と和) [persistent_range_affine_range_sum]
documentation_of: ../../../../src/structure/segtree/persistent/persistent_range_affine_sum.hpp
---

区間アフィン作用・区間和に加えて、別の version の区間をコピーする操作を持つ完全永続遅延セグメント木。

## API

- `PersistentRangeAffineSum<T>(values)`: 初期配列 `values` を version 0 とする。
- `affine(version, l, r, a, b)`: 指定 version を複製し、半開区間 $[l, r)$ の各要素 $x$ を $ax+b$ に置き換えた新しい version 番号を返す。
- `copy_range(destination, source, l, r)`: `destination` の区間 $[l, r)$ を `source` の同じ区間で置き換えた新しい version 番号を返す。
- `range_sum(version, l, r)`: 指定 version の区間 $[l, r)$ の和を返す。
- `size()`, `versions()`: 列の長さと version 数を返す。

## 時間計算量

$N$ を列の長さとする。構築は $O(N)$、更新・コピー・クエリは各 $O(\log(N+1))$ である。$Q$ 回の更新・コピー後の保存領域は $O(N+Q\log(N+1))$ である。空区間の更新は $O(1)$ である。

## 注意点

- 区間は半開区間で、version 番号は `0 <= version < versions()` を満たす必要がある。違反時は `std::out_of_range` を送出する。
- 乗数 0 を許し、逆元は要求しない。既存の version は変更しない。
- 空区間の更新・コピーも新しい version を 1 つ作る。
- `T` は整数からの構築・比較・加算・乗算に対応し、すべての中間値を表現できる必要がある。

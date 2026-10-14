---
title: Persistent Range Affine Sum with Copy (区間コピー付き永続アフィン作用と和) [persistent_range_affine_range_sum]
documentation_of: ../../../../src/structure/segtree/persistent/persistent_range_affine_sum.hpp
---

`PersistentRangeAffineSum<T>(values)` は初期配列を版 0 とする。

- `affine(version,l,r,a,b)`: 指定版を複製し、区間へ `a*value+b` を作用させた新しい版番号を返す。
- `copy_range(destination,source,l,r)`: destination の指定区間を source の同じ区間で置き換え、新しい版番号を返す。
- `range_sum(version,l,r)`: 指定版の区間和。
- `size()`, `versions()`。

## 時間計算量

構築 $O(N)$、更新・コピー・照会は各 $O(\log(N+1))$。
$Q$ 回の更新・コピー後の保存空間は $O(N+Q\log(N+1))$。空区間更新は $O(1)$。

## 注意点

区間は半開、版番号は `0<=version<versions()`。違反時は `out_of_range`。
零の乗数を許し、逆元を要求しない。既存版は更新しない。
`T` は整数からの構築、比較、加算、乗算が必要。全中間値を表現できること。

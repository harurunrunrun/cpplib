---
title: Sortable Monoid Sequence (キー順ソート対応モノイド列) [point_set_range_sort_range_composite]
documentation_of: ../../../../src/structure/bbst/sequence/sortable_monoid_sequence.hpp
---

`SortableMonoidSequence<Monoid>(key_limit, keys, values)` は、相異なる整数キーを持つ要素の列を保持し、区間ソートと区間の積を扱う。`Monoid` は `S`, `e()`, `op(a, b)` を持つモノイドのオブジェクト（テンプレート引数 `auto Monoid`）である。

## API

- `SortableMonoidSequence(key_limit, keys, values)`: 位置 $i$ の要素がキー `keys[i]`、値 `values[i]` を持つ列を構築する。
- `size()`: 列の長さ $N$ を返す。
- `set(position, key, value)`: 位置 `position` の要素のキーと値を置き換える。
- `sort(l, r, descending = false)`: 半開区間 $[l,r)$ をキーの昇順（`descending` なら降順）に並べ替える。
- `prod(l, r)`: 半開区間 $[l,r)$ の値を現在の列順に掛け合わせる。
- `prod_key_range(l, r, key_lower, key_upper)`: 位置区間 $[l,r)$ のうちキーが [`key_lower`, `key_upper`) に入る要素だけを、列順に掛け合わせる。

## 時間計算量

キーの値域の幅を $U$（`key_limit`）とし、モノイド演算を $O(1)$ とする。

- 構築: $O(N(\log U+\log(N+1)))$
- $Q$ 回の操作の合計: $O((N+Q)\log(U+1)\log(N+1))$
- 1 回の `sort` は最悪 $O(N\log(U+1)\log(N+1))$ になり得るが、ブロックの生成・消去で償却される。
- `prod_key_range`: 位置区間内のソート済みブロック数を $K$ として $O((K+1)\log(U+1)+\log(N+1))$
- 空間計算量: $O(N\log(U+1))$

## 注意点

- キーは常に相異なり、$0\le\mathrm{key}<U$ でなければならない。範囲外のキーや位置・区間は `std::out_of_range` を投げる。`key_limit` が正でないか `keys` と `values` の長さが異なる場合は `std::invalid_argument` を投げる。
- 重複キーは契約違反であり、構築時には検査しない（後の併合で検出された場合は `std::invalid_argument` を投げる）。
- 値の演算は非可換でもよい。
- 積を求める操作も内部のブロック分割を行うので非 `const` である。空区間の積は単位元。

---
title: Sortable Monoid Sequence (キー順ソート対応モノイド列) [point_set_range_sort_range_composite]
documentation_of: ../../../../src/structure/bbst/sequence/sortable_monoid_sequence.hpp
---

`SortableMonoidSequence<Monoid>(key_limit,keys,values)` は相異なる整数キー付きの列を保持する。
`Monoid` は既存の `Monoid` 形式で `S`, `e()`, `op(a,b)` を提供する。

- `set(position,key,value)`: キーと値を置き換える。
- `sort(l,r,descending=false)`: 半開区間をキーの昇順または降順に並べる。
- `prod(l,r)`: 現在の列順に値を集約する。
- `prod_key_range(l,r,key_lower,key_upper)`: 指定位置区間内でキーが `[key_lower,key_upper)` の要素だけを、列順に集約する。
- `size()`。

## 時間計算量

キー領域幅を $U$ とする。構築 $O(N(\log U+\log(N+1)))$、
$Q$ 回の操作合計は $O((N+Q)\log(U+1)\log(N+1))$、保存空間 $O(N\log(U+1))$。
単一の sort は最悪 $O(N\log(U+1)\log(N+1))$ となり得るが、block の生成消去で償却される。
以上はモノイド演算を定数時間とする。
キーで絞る集約は対象位置区間内の sorted block 数を $K$ として $O((K+1)\log(U+1)+\log(N+1))$。

## 注意点

キーは常に相異なり `0<=key<key_limit` であること。範囲外は `out_of_range`。
重複キーは契約違反であり、構築時には検査しない。値の演算は非可換でもよい。
集約照会も内部の block 分割を行うため非 `const`。空区間の積は単位元。

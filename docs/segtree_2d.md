---
title: 2D Segment Tree (二次元セグメント木)
documentation_of: ../src/structure/segtree/segtree_2d.hpp
---

固定最大サイズの二次元segment tree。点更新と長方形積を扱う。

```cpp
constexpr AddMonoid<long long> monoid{};
Segtree2D<monoid, MAX_HEIGHT, MAX_WIDTH> seg(values);

seg.set(row, col, value);
seg.apply(row, col, delta);
seg.prod(row_begin, col_begin, row_end, col_end);
```

長方形は `[row_begin, row_end) x [col_begin, col_end)` の半開区間である。
高速な二次元分割では部分長方形を結合する順序が一意でないため、`Monoid.op` は
結合的かつ可換でなければならない。`Monoid.e()` は単位元を返す。

## 計算量

$R,C$ を `MAX_HEIGHT`, `MAX_WIDTH` 以上の最小の2冪、
$H,W$ を実際の高さ・幅とする。Monoid操作1回を $O(1)$ と数える。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`Segtree2D(height, width)` | $O(RC)$ |
| `Segtree2D(values)` | $O(RC)$ |
| `build(values)` | $O(RC)$ |
| `height()`, `width()`, `empty()` | $O(1)$ |
| `get(row, col)` | $O(1)$ |
| `set(row, col, value)`, `apply(row, col, value)` | $O(\log R\log C)$ |
| `prod(row_begin, col_begin, row_end, col_end)` | $O(\log R\log C)$ |
| `all_prod()` | $O(1)$ |

保存領域は $O(RC)$。ただし実装はtemplate上限から決まる固定長 `std::array` を持つため、
コピー構築・コピー代入も $O(RC)$ となる。

# API契約

`height/width` は実際の形、`empty` は少なくとも一方が0かを返す。
`get` は1点、`set` は置換、`apply` は現在値との `Monoid.op` による1点更新、
`prod` は半開矩形、`all_prod` は全体を返す。`build(values)` では全行の幅が同じ必要がある。

高さ・幅はtemplate容量以下でなければならない。点・矩形の範囲、逆区間、非矩形vector、
容量超過では `runtime_error`。各APIの計算量は表の通り。

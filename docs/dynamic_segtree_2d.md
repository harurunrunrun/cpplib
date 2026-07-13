---
title: Dynamic 2D Segment Tree
documentation_of: ../src/structure/segtree/dynamic_segtree_2d.hpp
---

必要な経路のnodeだけを生成する疎な二次元segment tree。
座標領域は `[0, MAX_HEIGHT) x [0, MAX_WIDTH)` で、点更新と長方形積を扱う。

```cpp
constexpr AddMonoid<long long> monoid{};
DynamicSegtree2D<monoid, 1000000000LL, 1000000000LL> seg;

seg.set(row, col, value);
seg.apply(row, col, delta);
seg.prod(row_begin, col_begin, row_end, col_end);
```

`Monoid.op` は結合的かつ可換でなければならない。未生成点の値は `Monoid.e()`。
一度生成したnodeは値を単位元へ戻しても残り、`clear()` でまとめて解放される。

## 計算量

$H=MAX\_HEIGHT$, $W=MAX\_WIDTH$ とし、Monoid操作1回を $O(1)$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| 構築 | $O(1)$ |
| `set(row, col, value)` | $O(\log H\log W)$ |
| `apply(row, col, value)` | $O(\log H\log W)$ |
| `get(row, col)` | $O(\log H+\log W)$ |
| `prod(row_begin, col_begin, row_end, col_end)` | $O(\log H\log W)$ |
| `all_prod()` | $O(1)$ |
| `row_node_count()`, `column_node_count()` | $O(1)$ |
| `clear()`、破棄 | $O(R+C)$ |

$R,C$ はそれまでに生成したrow node数・column node数。
1回の `set` が新しく生成するnode数は $O(\log H\log W)$、保存領域は $O(R+C)$。
コピー構築・コピー代入は禁止している。

# API契約

`get` は1点、`set` は置換、`apply` は現在値との `Monoid.op` による1点更新、
`prod` は半開矩形、`all_prod` は全領域を返す。`row_node_count/column_node_count` は
確保済みnode数。`clear` 後は全点が単位元でnode数も0になる。

点・矩形の範囲違反と逆区間は `runtime_error`、確保失敗は `bad_alloc`。
copyは禁止される。各APIの計算量は表の通り。

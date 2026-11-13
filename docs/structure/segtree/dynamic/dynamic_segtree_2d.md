---
title: Dynamic 2D Segment Tree (動的二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/dynamic/dynamic_segtree_2d.hpp
---

必要な経路のノードだけを生成する、疎な 2 次元のセグメント木である。座標の範囲は `[0, MAX_HEIGHT) x [0, MAX_WIDTH)` で、1 点の更新と長方形領域の積を扱う。

```cpp
template<auto Monoid, long long MAX_HEIGHT = -1, long long MAX_WIDTH = -1>
struct DynamicSegtree2D;

constexpr AddMonoid<long long> monoid{};
DynamicSegtree2D<monoid, 1000000000LL, 1000000000LL> seg;

seg.set(row, col, value);
seg.apply(row, col, delta);
seg.prod(row_begin, col_begin, row_end, col_end);
```

`Monoid.op` は結合的かつ可換でなければならない。まだ生成されていない点の値は `Monoid.e()` である。一度生成したノードは値を単位元に戻しても残り、`clear()` でまとめて解放される。

## API

- `get(row, col)`: 1 点の値を返す。
- `set(row, col, value)`: 1 点を `value` に置き換える。
- `apply(row, col, value)`: 1 点を、現在の値と `value` の `Monoid.op` の結果に置き換える。
- `prod(row_begin, col_begin, row_end, col_end)`: 半開の長方形 `[row_begin, row_end) x [col_begin, col_end)` の積を返す。
- `all_prod()`: 全領域の積を返す。
- `row_node_count()`, `column_node_count()`: 確保済みの行ノード数・列ノード数を返す。
- `clear()`: すべてのノードを解放する。その後は全点が単位元で、ノード数も 0 になる。

## 時間計算量

$H=\texttt{MAX_HEIGHT}$, $W=\texttt{MAX_WIDTH}$ とし、モノイドの演算 1 回を $O(1)$ とする。

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

$R, C$ はそれまでに生成した行ノード数・列ノード数である。1 回の `set` で新しく生成するノード数は $O(\log H\log W)$、保存領域は $O(R+C)$ である。

## 注意点

- 点・長方形の範囲違反と逆向きの区間では `std::runtime_error` を、ノードの確保に失敗すると `std::bad_alloc` を送出する。
- コピー構築・コピー代入はできない。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`DynamicSegtree2D<Monoid> tree(height, width);` のように両方の容量を省略すると、同じヘッダ内の可変長版を使い、座標の範囲は `[0, height) x [0, width)` になる。このコンストラクタは `explicit DynamicSegtree2D(long long height = 0, long long width = 0)` で、負または大きすぎる値では `std::length_error` を送出する。

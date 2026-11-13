---
title: 2D Segment Tree (二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/basic/segtree_2d.hpp
---

2 次元のセグメント木である。1 点の更新と長方形領域の積を扱う。

```cpp
template<auto Monoid, int MAX_HEIGHT = -1, int MAX_WIDTH = -1>
struct Segtree2D;

constexpr AddMonoid<long long> monoid{};
Segtree2D<monoid, MAX_HEIGHT, MAX_WIDTH> seg(values);

seg.set(row, col, value);
seg.apply(row, col, delta);
seg.prod(row_begin, col_begin, row_end, col_end);
```

長方形は `[row_begin, row_end) x [col_begin, col_end)` の半開区間で指定する。2 次元の分割では部分長方形を結合する順序が一意でないので、`Monoid.op` は結合的かつ可換でなければならない。`Monoid.e()` は単位元を返す。

## API

- `Segtree2D()`: 大きさ $0 \times 0$ で構築する。
- `Segtree2D(int height, int width)`: 全要素が単位元の $\mathit{height} \times \mathit{width}$ の行列で構築する。
- `explicit Segtree2D(const vector<vector<S>>& values)`, `build(values)`: `values` で構築する。全行の幅が同じである必要がある。
- `height()`, `width()`: 実際の行数・列数を返す。`empty()` は少なくとも一方が 0 かどうかを返す。
- `get(row, col)`: 1 点の値を返す。
- `set(row, col, value)`: 1 点を `value` に置き換える。
- `apply(row, col, value)`: 1 点を、現在の値と `value` の `Monoid.op` の結果に置き換える。
- `prod(row_begin, col_begin, row_end, col_end)`: 半開の長方形の積を返す。
- `all_prod()`: 全体の積を返す。

## 時間計算量

$R, C$ をそれぞれ `MAX_HEIGHT`, `MAX_WIDTH` 以上の最小の 2 冪とし、$H, W$ を実際の高さ・幅とする。モノイドの演算 1 回を $O(1)$ と数える。

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

保存領域は $O(RC)$ である。固定長版はテンプレート引数の上限から決まる固定長の `std::array` を持つので、コピー構築・コピー代入も $O(RC)$ となる。

## 注意点

- 高さ・幅はテンプレート引数の容量以下でなければならない。
- 点・長方形の範囲外、逆向きの区間、行ごとに長さが異なる `vector`、容量超過では `std::runtime_error` を送出する。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`Segtree2D<Monoid> tree(height, width);` のように容量を省略すると、同じヘッダ内の可変長版を使う。この場合 $R, C$ は実際の $H, W$ 以上の最小の 2 冪になり、領域は実行時に確保する。

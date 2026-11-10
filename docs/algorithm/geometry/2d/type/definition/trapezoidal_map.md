---
title: Trapezoidal Map (台形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/trapezoidal_map.hpp
---

線分の端点の $x$ 座標で平面をスラブに分割し、点の直下・直上にある線分を検索する。
各スラブにおける線分の順序は、構造を共有する永続 AVL 木で保持するため、全スラブへ線分列を複製しない。

## API

### `TrapezoidalMap()`, `explicit TrapezoidalMap(std::vector<Segment> segments)`

デフォルトコンストラクタは空の検索構造を作る。
線分列を受け取るコンストラクタは、線分番号を入力順の 0-indexed として検索構造を構築する。端点の座標が有限でない場合や、両端が
同一点と判定される退化線分には `std::invalid_argument` を送出する。

### `TrapezoidalMapLocation locate(const Point& point) const`

`point` を含むスラブの番号 `slab` と、直下の線分 `below`、直上の線分 `above`、点が載る線分 `on_segment` の番号を返す。
該当する線分がなければ `TrapezoidalMapLocation::no_segment` となる。

点が線分上にある場合は `on_segment` にその番号が入る。このとき `below`, `above` は
規定しない（点が端点上・垂直線分上にある場合は `no_segment` のまま、それ以外の場合は探索で得た線分が入り得る）。

端点の $x$ 座標と一致し、かつ線分上にない点には、その $x$ 座標の直後にある開スラブを用いる。
垂直線分は `on_segment` の候補にはなるが、`below` と `above` を求める順序木には入らない。

### `std::size_t segment_count() const`

入力線分の本数を返す。

### `std::size_t slab_count() const`

保持するスラブの根の個数を返す。線分が 1 本以上あれば、相異なる端点の $x$ 座標の個数に $1$ を加えた値である。
空の入力では $0$ を返す。

## 時間計算量

線分数を $N$ とする。

- 構築: 最悪 $O(N \log N)$ 時間、$O(N \log N)$ 空間。
- `locate`: 最悪 $O(\log N)$。線分上かどうかの判定は、端点の辞書、垂直線分の辞書、または AVL 木の探索で得た
  直上・直下の高々 2 本だけを調べ、全線分の走査は行わない。
- `segment_count`, `slab_count`: $O(1)$。

## 注意点

- 入力線分の相対的な内部は互いに交わってはならず、重なってもならない。異なる線分が共有してよい
  点は共通の端点だけである。この前提により、各開スラブ内で線分の上下の順序が一定になる。
- 構築時にはこの前提をすべての組について検査しない（検査すると 2 乗時間になる）。前提に違反する入力に対する結果は未規定である。
- 幾何的な比較には共通の相対許容誤差を使う。同じ点が複数の線分の共通端点である場合、
  `on_segment` が返す線分番号はそのうちのいずれかである。

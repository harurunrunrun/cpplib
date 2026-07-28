---
title: Trapezoidal Map (台形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/trapezoidal_map.hpp
---

線分端点のx座標で平面をslabへ分割し、点の直下・直上の線分を検索する。
各slabの線分順序は構造共有する永続AVL木で保持するため、全slabへ線分列を複製しない。

## API

### `TrapezoidalMap(std::vector<Segment> segments)`

線分番号を入力順の0始まりとして検索構造を構築する。有限座標でない端点、および両端が
同一点と判定される退化線分には `std::invalid_argument` を送出する。

### `TrapezoidalMapLocation locate(const Point& point) const`

`point` を含むslab番号と、直下 `below`、直上 `above`、点上 `on_segment` の線分番号を返す。
該当線分がなければ `TrapezoidalMapLocation::no_segment` とする。点が線分上なら
`on_segment` だけを保証し、`below` と `above` は `no_segment` のままとする。

端点x座標と一致し、かつ線分上でない点には、そのx座標の直後にある開slabを用いる。
垂直線分は `on_segment` の候補にはなるが、`below` と `above` の順序木には入らない。

### `std::size_t segment_count() const`

入力線分数を返す。

### `std::size_t slab_count() const`

保持するslab根の数を返す。線分が1本以上なら、異なる端点x座標数に1を加えた値である。
空入力では0を返す。

## 時間計算量

線分数を $N$ とする。

- 構築: 最悪 $O(N\log N)$ 時間、$O(N\log N)$ 空間。
- `locate`: 最悪 $O(\log N)$。線分上判定は端点辞書、垂直線分辞書、またはAVL探索で得た
  直上・直下の高々2本だけを調べ、全線分走査を行わない。
- `segment_count`, `slab_count`: $O(1)$。

## 注意点

入力線分の相対内部は互いに交わってはならず、重なってもならない。異なる線分が共有できる
交点は共通端点だけである。この前提により、各開slab内で線分の上下順序が一定になる。
構築時にはこの全組の前提を検査しない。検査すると二次時間になるため、違反入力での結果は未規定とする。
幾何比較には共通の相対許容誤差を使う。同じ点が複数線分の共通端点なら、
`on_segment` が返す線分番号はそのいずれかである。

---
title: Convex Polyhedron Halfspace Clip (凸多面体の半空間切断)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_plane/convex_polyhedron_clip_halfspace.hpp
---

## API

- `convex_polyhedron_clip_halfspace(polyhedron, boundary, keep_negative_side = true)`: `boundary` の点を $p$、法線を $n$ として、`keep_negative_side` が `true` なら $n \cdot (x - p) \le 0$ 側、`false` なら $n \cdot (x - p) \ge 0$ 側の閉半空間と凸多面体の共通部分を、凸包として返す。

## API 別の時間計算量・空間計算量

辺数を $E$、候補点（残す側の頂点と境界をまたぐ辺の交点）の数を $K$ として、時間 $O(E \log E + K \log K)$、領域 $O(E + K)$。辺の列挙が $O(E \log E)$、候補点の凸包構築（`convex_hull_3d`）が $O(K \log K)$ である。

## 注意点

- 共通部分が空なら `affine_dimension == -1` の多面体を返す。境界上の頂点はどちらの側を残す場合にも含める。
- 側の判定は、法線を正規化せずに exact dyadic 演算（相対許容誤差付き）で行う。境界をまたぐ辺の交点を求め、候補点から凸包を再構成する。
- 零法線、非有限な入力には `std::invalid_argument` を送出し、表現できない交点には `std::overflow_error` を送出する。

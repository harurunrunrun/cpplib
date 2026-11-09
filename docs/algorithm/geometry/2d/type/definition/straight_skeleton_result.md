---
title: Straight Skeleton Result (直線骨格結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/straight_skeleton_result.hpp
---

直線骨格（straight skeleton）を、頂点列、各頂点が生じたオフセット時刻、頂点の種別、無向辺の列で格納する。

## API

```cpp
enum class StraightSkeletonVertexKind : std::uint8_t {
    contour_vertex, edge_event, split_event, peak_event, simultaneous_event,
};

struct StraightSkeletonResult {
    std::vector<Point> vertices;
    std::vector<long double> times;
    std::vector<StraightSkeletonVertexKind> kinds;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
};
```

- `StraightSkeletonVertexKind` は `contour_vertex`、`edge_event`、`split_event`、
  `peak_event`、`simultaneous_event` のいずれかである。種類の異なるイベントが同一点・
  同時刻に重なった場合は `simultaneous_event` になる。
- `vertices[i]` は骨格の頂点の座標である。入力頂点は同じ順序で先頭に格納される。
- `times[i]` は、波面（wavefront）が `vertices[i]` に到達したときの非負のオフセット距離である。
  入力頂点では $0$ である。
- `kinds[i]` は `vertices[i]` の種別である。
- `edges` は骨格の無向辺を頂点番号の組で格納する。自己ループと重複辺は含まない。

常に `vertices.size() == times.size() == kinds.size()` が成り立ち、`edges` の
両端は有効な頂点番号である。
単純多角形から構築した結果では、`edges` は全頂点を結ぶ木をなす。

## 時間計算量

各メンバの参照は $O(1)$、全走査は格納された頂点数と辺数に対して線形。

## 注意点

`times` は辺に沿った弧長ではなく、入力辺を内側へ平行移動した距離である。

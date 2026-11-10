---
title: Advanced Geometry Result Types (高度幾何の結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/half_plane_intersection_result.hpp
---

半平面の共通部分（半平面交差）の結果型。

## API

```cpp
enum HalfPlaneIntersectionKind {
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult {
    HalfPlaneIntersectionKind kind = HALF_PLANE_EMPTY;
    std::vector<Point> polygon;
};
```

- `HalfPlaneIntersectionKind`: 共通部分が空、有界、非有界のいずれかを表す unscoped 列挙型。
- `HalfPlaneIntersectionResult`: 分類 `kind` と、有界のときの頂点列 `polygon` をもつ。

## 時間計算量・空間計算量

- 列挙値の参照と空の結果の構築: 時間・追加空間 $O(1)$。
- `polygon` のコピー: 頂点数を $H$ として時間・空間 $O(H)$。

## 注意点

非有界または空の結果では `polygon` は空である。このヘッダは高度な凸幾何 API で共通に使う結果型を定義する。

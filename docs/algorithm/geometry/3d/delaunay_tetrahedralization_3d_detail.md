---
title: 3D Delaunay Tetrahedralization Internal Detail (三次元Delaunay四面体分割内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/delaunay_tetrahedralization_3d_detail.hpp
---

Bowyer--Watson本体、conflict graph、adaptive predicate、完了検査を共有する。

## API

- `delaunay_tetrahedralization_3d_detail::build_randomized(points, seed)`: 二つの公開wrapperから呼ばれる内部構築本体。
- その他のdetail型・関数: 空洞更新、super tetra拡大、体積一致検査を実装する。

## API別の時間計算量・空間計算量

一般位置の乱択順では期待 $O(N\log N+K)$ 時間・$O(N+K)$ 領域。任意順では $O(N\log N+K+C)$ 時間で、最悪 $K=O(N^2)$、$C=O(NK)$。

## 注意点

内部実装用headerであり互換性を保証しない。公開APIにはdefaultまたはrandomized leafを使う。

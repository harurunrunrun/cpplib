---
title: 3D Point Deduplication Internal Detail (三次元点重複除去内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/deduplicate_points_3d_detail.hpp
---

exact sort、固定幅cell、代表点走査、相対範囲木の共有実装を保持する。

## API

- `deduplicate_points_3d_detail` 内の型・関数: exact dyadic cell番号、許容区間、三重Fenwick構造を実装する内部要素。

## API別の時間計算量・空間計算量

exact sortは $O(N\log N)$、固定幅cellは $O(N\log U)$、代表点走査は $O(NU)$、相対範囲木は $O(N\log^3 N)$ 時間・$O(N\log^2 N)$ 領域。

## 注意点

内部実装用headerであり互換性を保証しない。公開APIには既定法またはrelative range treeのleafを使う。

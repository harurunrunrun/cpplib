---
title: 3D Point Deduplication Internal Detail (三次元点重複除去内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/deduplicate_points_3d_detail.hpp
---

正確なソート、固定幅のセル、代表点の走査、相対範囲木による各重複除去法の共有実装である。

## API

- `deduplicate_points_3d_detail` 内の型・関数: 正確な 2 進有理数（dyadic）によるセル番号、許容区間、三重の Fenwick 木を実装する内部要素。

## API別の時間計算量・空間計算量

入力点数を $N$、残る代表点数を $U$ とする。

- 正確なソート（両方の許容誤差が 0）: 時間 $O(N\log N)$。
- 固定幅のセル（相対許容誤差が 0）: 時間 $O(N\log U)$。
- 代表点の走査（相対許容誤差が正）: 時間 $O(NU)$。
- 相対範囲木: 時間 $O(N\log^3 N)$、領域 $O(N\log^2 N)$。

## 注意点

内部実装用のヘッダであり、互換性を保証しない。公開 API としては、デフォルトの方法または相対範囲木（relative range tree）の個別ヘッダを使うこと。

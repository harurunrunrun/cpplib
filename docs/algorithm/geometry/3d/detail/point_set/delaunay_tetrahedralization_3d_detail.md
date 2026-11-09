---
title: 3D Delaunay Tetrahedralization Internal Detail (三次元Delaunay四面体分割内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/delaunay_tetrahedralization_3d_detail.hpp
---

Bowyer–Watson 法の本体、conflict グラフ、適応的精度の述語、完了検査を共有する内部実装である。

## API

- `delaunay_tetrahedralization_3d_detail::build_randomized(points, seed)`: 2 つの公開ラッパーから呼ばれる構築本体。座標が完全一致する重複点を除き、アフィン次元が 3 未満なら四面体を空にして返す。
- その他の内部の型・関数: 空洞の更新、外側の超四面体（super tetrahedron）の拡大、体積の一致による分割の完全性検査を実装する。

## API別の時間計算量・空間計算量

点数を $N$、生成する四面体と conflict 関係の総数を $K$、検査する conflict の組の数を $C$ とする。一般位置で乱択順なら期待時間 $O(N\log N+K)$、領域 $O(N+K)$。任意の順序では時間 $O(N\log N+K+C)$ で、最悪 $K=O(N^2)$、$C=O(NK)$ となる。

## 注意点

内部実装用のヘッダであり、互換性を保証しない。公開 API としては、デフォルト版またはランダム化版の個別ヘッダを使うこと。

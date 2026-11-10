---
title: 3D Halfspace Intersection Internal Detail (三次元半空間交差内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/plane_point/halfspace_intersection_3d_detail.hpp
---

2 つの公開個別ヘッダが共有する、実行可能性判定・内点探索・極双対からの復元を実装する。

## API

- 名前空間 `halfspace_intersection_3d_detail` 内の関数・型: 公開ラッパーから使う、固定次元の実行可能性探索、退化処理、双対凸包の構築、主問題の頂点復元を実装する内部要素。

## API別の時間計算量・空間計算量

各公開 API 全体の計算量は、対応する個別ヘッダのドキュメントに記載する。半空間数を $H$、出力サイズを $K$ とすると、内部の極双対構築は入力に依存しない順序で期待時間 $O(H\log H+K)$、領域 $O(H+K)$ である。
完全な退化処理を含めると、最悪時間 $O(H^4+K)$、領域 $O(H+K)$ となる。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
利用側は 2 つの公開個別ヘッダ、または互換用の集約ヘッダを include すること。

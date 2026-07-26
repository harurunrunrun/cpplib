---
title: 3D Halfspace Intersection Internal Detail (三次元半空間交差内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/halfspace_intersection_3d_detail.hpp
---

二つの公開leafが共有する実行可能性判定、内点探索、極双対復元を保持する。

## API

- `halfspace_intersection_3d_detail` 内の関数・型: 公開wrapperから使う
  固定次元実行可能性探索、退化処理、双対凸包構築、主問題頂点復元の内部要素。

## API別の時間計算量・空間計算量

各公開API全体の計算量は対応するleafのdocsに記載する。内部の極双対構築は
入力独立な順序で期待 $O(H\log H+K)$ 時間・$O(H+K)$ 領域、
完全な退化処理は最悪 $O(H^4+K)$ 時間・$O(H+K)$ 領域。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
利用側は二つの公開leafまたは互換aggregatorをincludeする。

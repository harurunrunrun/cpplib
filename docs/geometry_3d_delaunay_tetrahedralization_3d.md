---
title: 3D Delaunay Tetrahedralization (三次元Delaunay四面体分割)
documentation_of: ../src/algorithm/geometry/3d/delaunay_tetrahedralization_3d.hpp
---

ランダム増分 Bowyer--Watson 法で有限点集合の Delaunay 四面体分割を構築する。
各四面体の隣接関係と未挿入点との conflict graph を保持し、点位置探索後は実際の
空洞だけを走査する。包含球判定には `adaptive_insphere`、向き判定には
`adaptive_orient3d` を使う。

## API

- `delaunay_tetrahedralization_3d(points)`: 完全一致する重複点を除き、
  `DelaunayTetrahedralization3` を返す。共球面では厳密に球内部の四面体だけを空洞から
  削除する。固定された挿入 seed を使うため、同じ入力には決定的に同じ分割を返す。
- `delaunay_tetrahedralization_3d_randomized(points, seed)`: 指定した `seed` で挿入順を
  シャッフルして構築する。同じ `points` と `seed` に対する結果は決定的である。

super tetra を除いた後、全入力点が使われること、面の接続数、四面体群と入力凸包の
厳密な体積一致を線形時間で検査する。有限 super tetra の影響が残った場合は scale を
拡大して再構築する。

## API別の時間計算量・空間計算量

最終出力と増分中に作られる四面体・conflict 関係の総数を $K$ とする。ランダムな
挿入順と一般位置を仮定すると、`delaunay_tetrahedralization_3d_randomized` は期待
時間 $O(N\log N+K)$、追加領域 $O(N+K)$。三次元 Delaunay 分割では
$K=O(N^2)$ になり得る。固定 seed の既定 API を含む任意の挿入順に対しては、検査した
conflict 組の総数を $C$ として時間 $O(N\log N+K+C)$ であり、最悪 $C=O(NK)$。
完了検査は凸包構築を含め期待 $O(N\log N+K)$、super tetra 再構築回数は12以下の定数。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 共線・共面入力も受理するが、三次元四面体は生成しない。
- 重複判定は許容誤差ではなく座標の完全一致で行う。
- 位相判定は適応的精度で行う。表現可能なsuper tetraで境界を覆えないほど
  ill-conditionedな入力では `std::overflow_error` を送出する。
- affine dimensionはexact dyadic predicateで判定する。構築座標は先頭点をanchorとする
  局所差分を共通scaleへ正規化するため、巨大な共通移動量と薄い局所形状を同時に扱える。

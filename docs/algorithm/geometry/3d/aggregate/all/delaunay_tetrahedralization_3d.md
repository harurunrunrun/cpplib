---
title: 3D Delaunay Tetrahedralization (三次元Delaunay四面体分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/delaunay_tetrahedralization_3d.hpp
---

ランダム増分 Bowyer--Watson 法で、有限点集合の Delaunay 四面体分割を構築する。各四面体の隣接関係と、未挿入点との conflict graph を保持し、点位置探索の後は実際の空洞だけを走査する。包含球判定には `adaptive_insphere`、向き判定には `adaptive_orient3d` を使う。

このヘッダは固定シード版とシード指定版の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な構築法の個別ヘッダだけを include すればよい。

## API

```cpp
DelaunayTetrahedralization3 delaunay_tetrahedralization_3d(std::vector<Point3> points);
DelaunayTetrahedralization3 delaunay_tetrahedralization_3d_randomized(std::vector<Point3> points, std::uint64_t seed);
```

- `delaunay_tetrahedralization_3d(points)`: 座標が完全に一致する重複点を除き、`DelaunayTetrahedralization3` を返す。共球面の場合は、厳密に球の内部にある四面体だけを空洞から削除する。固定の挿入シードを使うため、同じ入力には常に同じ分割を返す。
- `delaunay_tetrahedralization_3d_randomized(points, seed)`: 指定した `seed` で挿入順をシャッフルして構築する。同じ `points` と `seed` に対する結果は決定的である。

super tetrahedron（入力全体を覆う補助四面体）を除いた後、すべての入力点が使われていること、面の接続数、四面体群と入力の凸包の体積が厳密に一致することを線形時間で検査する。有限の super tetrahedron の影響が残った場合は、スケールを拡大して再構築する。

## API 別の時間計算量・空間計算量

入力点数を $N$、最終出力と増分中に作られる四面体・conflict 関係の総数を $K$ とする。

- ランダムな挿入順と一般位置を仮定すると、`delaunay_tetrahedralization_3d_randomized` は期待時間 $O(N \log N + K)$、追加空間 $O(N + K)$。三次元の Delaunay 分割では $K = O(N^2)$ になり得る。
- 固定シードのデフォルト API を含め、任意の挿入順に対しては、検査した conflict の組の総数を $C$ として時間 $O(N \log N + K + C)$ であり、最悪 $C = O(NK)$。
- 完了時の検査は凸包の構築を含めて期待 $O(N \log N + K)$。super tetrahedron による再構築の回数は 12 回以下の定数である。

## 注意点

- 座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 共線・共面な入力も受け付けるが、三次元の四面体は生成しない。
- 重複判定は許容誤差ではなく、座標の完全一致で行う。
- 位相的な判定は適応的精度で行う。表現可能な super tetrahedron で境界を覆えないほど悪条件な入力では `std::overflow_error` を送出する。
- アフィン次元は正確な 2 進有理数による厳密な述語で判定する。構築に使う座標は、先頭点を基準とする局所的な差分を共通のスケールに正規化したものなので、巨大な共通の平行移動量と薄い局所形状を同時に扱える。

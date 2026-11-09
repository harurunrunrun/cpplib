---
title: Randomized 3D Voronoi Diagram (乱択三次元Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/voronoi_diagram_3d_randomized.hpp
---

指定したシードで構築した Delaunay 四面体分割（`delaunay_tetrahedralization_3d_randomized`）の双対として、三次元 Voronoi 図を構築する。

## API

- `voronoi_diagram_3d_randomized(points, seed)`: 増分構築の順序を `seed`（`std::uint64_t`）で指定し、有限頂点・セル・リッジ（Voronoi 面）・辺・無限半直線とそれらの接続関係を持つ `VoronoiDiagram3` を返す。

## API別の時間計算量・空間計算量

サイト数を $N$、Delaunay 構築の手間を $D$、出力と接続関係の総数を $K$ とする。シードを入力と独立に選んだ一般位置の入力では、ハッシュ操作を期待 $O(1)$ とみなすと、期待時間 $O(N \log N + D + K)$、期待追加空間 $O(N + D + K)$。出力サイズは最悪 $O(N^2)$。

## 注意点

- 同じ入力と同じシードには同じ結果を返す。シードは幾何的な Voronoi 領域を変えない。
- アフィン次元が 3 未満なら、サイトと空のセルレコードだけを返す。
- 外心が有限範囲を超える場合は `std::overflow_error` を送出する。座標が非有限の場合は `std::invalid_argument` を送出する。
- 格納順ではなく、接続関係の添字を通じて参照すること。

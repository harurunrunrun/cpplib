---
title: 3D Voronoi Diagram (三次元Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/voronoi_diagram_3d.hpp
---

三次元 Delaunay 四面体分割の双対として、描画や包含判定に使える Voronoi 図を構築する。

このヘッダは `voronoi_diagram_3d_default.hpp` と `voronoi_diagram_3d_randomized.hpp` をまとめて読み込む後方互換用の集約ヘッダである。
片方の API だけを使うなら、対応する個別ヘッダを直接 include すればよい。

## API

- `voronoi_diagram_3d(points)`: Voronoi 図を `VoronoiDiagram3` で返す。`long double` の 3 座標が完全一致する外心は最初の出現位置へ統合して有限頂点とし、各セルの二等分半空間、巡回順またはパス順のリッジ、有限な線分の辺、外向きの無限半直線の辺と、それらの全接続関係を返す。座標が完全一致する重複母点は除去する。Delaunay 分割の構築には固定シードを使うため、同じ入力には同じ分割を返す。
- `voronoi_diagram_3d_randomized(points, seed)`: 指定した `seed` によるランダムな挿入順で Delaunay 分割を構築し、同じ形式の Voronoi 図を返す。同じ入力と `seed` に対しては決定的である。

### `VoronoiDiagram3` のフィールド

- `affine_dimension`: 重複除去後の母点集合のアフィン次元。
- `sites`, `finite_vertices`: 重複除去済みの母点と、Delaunay 四面体に双対な有限 Voronoi 頂点。
- `cells`: 各母点のセル。`neighbors`、`ridge_indices`、`boundary_halfspaces` の同じ添字が、同じ隣接セルとの境界を表す。有界セルの `bounded_polyhedron` は、リッジの巡回頂点列から直接三角形分割して作る。
- `ridges`: Delaunay 辺に双対な二次元の境界。`finite_vertices` は、有界リッジでは巡回順、非有界リッジでは有限部分のパス順である。
- `edges`: Delaunay 面に双対な一次元の境界。`segment`（線分）または外向きの `ray`（半直線）のどちらか一方を持つ。共球退化では、1 点だけからなる長さ 0 の有限な辺もある。

境界にある Delaunay 面の法線は、反対側の四面体頂点と逆向きになるように統一する。座標差が大きい場合の差分と外積は、入力のスケールで正規化してから計算する。

## API別の時間計算量・空間計算量

重複除去後の母点数を $N$、Delaunay 四面体数を $T$、返す頂点・辺・リッジ・セル面とそれらの接続関係の総数を $K$ とする。
Delaunay の増分構築中に生成する四面体と保持する conflict 関係の総数を $D$、検査する conflict の組の数を $C$ とする。
ハッシュ表の操作は期待計算量で数える。$T \le D$ である。

| 処理 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 固定シード API の Delaunay 構築 | $O(N\log N+D+C)$ | $O(N+D)$ |
| ランダム化 API の Delaunay 構築（一般位置・期待値） | $O(N\log N+D)$ | $O(N+D)$ |
| 双対の接続関係と全有界セル面の直接構築（期待値） | $O(T+K)$ | $O(T+K)$ |
| `voronoi_diagram_3d` 全体（ハッシュ表の期待値） | $O(N\log N+D+C+K)$ | $O(N+D+K)$ |
| `voronoi_diagram_3d_randomized` 全体（一般位置・期待値） | $O(N\log N+D+K)$ | $O(N+D+K)$ |

外心、Delaunay 辺、Delaunay 面はハッシュ表による索引で一度ずつ集約する。有界セルの面は双対リッジの巡回列を扇形に分割して作るため、セルごとに高次数の半空間交差を実行することはない。三次元の Delaunay 分割・Voronoi 図は、出力自体が最悪 $K=O(N^2)$ になり得る。

## 注意点

- アフィン次元が 3 未満なら、三次元の双対としての有限頂点・リッジ・辺は空にし、母点と空のセルだけを返す。低次元の Voronoi 図へ射影する API ではない。
- 非有界セルでも `boundary_halfspaces` を使えば包含判定ができる。
- 有界セルの `bounded_polyhedron` は双対の接続関係から構築する。共球な場合に生じる補助的な Delaunay 辺に双対な面積 0 のリッジはセル面から除外するが、接続関係の記録には残す。
- ハッシュ表を用いるため、辺・リッジの格納順は API の契約に含まれない。接続は必ず添字のフィールドを通して参照すること。
- 外心が有限の範囲を超える場合は `std::overflow_error` を送出する。
- 四面体の外心は入力座標に対する正確な 2 進有理数（dyadic）の Cramer の公式で求め、最終的な座標だけを `long double` へ丸める。境界面と二等分面は基準点からの局所差分を正規化して作るため、巨大な共通の平行移動と非常に薄い局所形状を同時に扱える。

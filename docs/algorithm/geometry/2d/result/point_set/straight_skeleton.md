---
title: Straight Skeleton (直線骨格)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/straight_skeleton.hpp
---

単純多角形の各辺を内側へ単位速度で平行移動するwavefrontを追跡し、wavefront頂点の軌跡から直線骨格を構築する。隣接頂点が衝突するedge event、凹頂点が非隣接辺へ衝突するsplit event、同時刻に起きる複数event、逆向き平行辺が重なるeventを処理する。

## API

`StraightSkeletonResult straight_skeleton(std::vector<Point> polygon)`

- `polygon` は時計回りまたは反時計回りの単純多角形である。末尾に先頭点をもう一度指定してもよく、その1点は除いて処理する。
- edge eventでは消滅するwavefront頂点の軌跡をevent頂点へ接続する。
- split eventでは凹頂点の軌跡を衝突点へ接続し、wavefrontを2成分に分けて伝播を続ける。
- 同時eventは同一offset時刻でまとめて処理する。逆向き平行辺の重複区間は同時刻の骨格辺として結果に含める。
- 同方向かつ同一直線上のwavefront辺が隣接した場合は、その継ぎ目を明示的なwavefront頂点として保持し、継ぎ目の軌跡も骨格へ接続する。
- 単純多角形に対する出力グラフは連結な木であり、同時eventや平行辺の縮退でも自己辺・重複辺を作らない。
- 戻り値の頂点、offset時刻、event種別、無向辺の契約は `StraightSkeletonResult` を参照する。

## 時間計算量

入力頂点数を $N$、出力頂点・辺の総数を $K$ とする。

- 入力の単純性検査: $O(N^2)$ 時間。
- 次eventの探索: 1 eventあたり $O(N^2)$ 時間。
- 全構築: $O(N^3)$ 時間、$O(N + K)$ 追加領域。単純多角形では $K=O(N)$ である。

## 注意点

隣接する同一点、一直線上の隣接3頂点、自己交差、面積0、非有限座標は `std::invalid_argument` で拒否する。穴は受け付けない。座標とevent時刻は `long double` で計算するため、極端に近いeventの分類は浮動小数点精度に依存する。一般位置は要求せず、矩形のような同時edge event、同時に複数のsplitが起きる凹多角形、平行辺の線分collapseを扱う。

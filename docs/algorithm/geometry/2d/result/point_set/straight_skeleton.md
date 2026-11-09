---
title: Straight Skeleton (直線骨格)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/straight_skeleton.hpp
---

単純多角形の各辺を内側へ単位速度で平行移動させた波面（wavefront）を追跡し、波面の頂点の軌跡から直線骨格を構築する。隣接頂点どうしが衝突する edge event、凹頂点が非隣接辺に衝突する split event、同時刻に起きる複数の event、逆向きの平行辺どうしが重なる event を処理する。

## API

```cpp
StraightSkeletonResult straight_skeleton(std::vector<Point> polygon);
```

- `polygon` は時計回りまたは反時計回りの単純多角形である。末尾に先頭と同じ点をもう一度指定してもよく、その場合は末尾の 1 点を除いて処理する。
- edge event では、消滅する波面頂点の軌跡を event 頂点へ接続する。
- split event では、凹頂点の軌跡を衝突点へ接続し、波面を 2 つの成分に分けて伝播を続ける。
- 同時に起きる event は同じオフセット時刻でまとめて処理する。逆向きの平行辺が重なる区間は、同時刻の骨格辺として結果に含める。
- 同じ向きかつ同一直線上の波面の辺が隣接した場合は、その継ぎ目を明示的な波面頂点として保持し、継ぎ目の軌跡も骨格へ接続する。
- 単純多角形に対する出力グラフは連結な木であり、同時 event や平行辺による縮退があっても自己ループや重複辺を作らない。

返り値 `StraightSkeletonResult` は次のメンバを持つ。

- `vertices`: 骨格の頂点の座標。
- `times`: 各頂点のオフセット時刻（輪郭の頂点では 0）。
- `kinds`: 各頂点の種別（`contour_vertex`, `edge_event`, `split_event`, `peak_event`, `simultaneous_event`）。
- `edges`: 頂点添字の組による無向辺。

## 時間計算量

入力の頂点数を $N$、出力の頂点数と辺数の合計を $K$ とする。

- 入力の単純性の検査: $O(N^2)$ 時間
- 次の event の探索: 1 event あたり $O(N^2)$ 時間
- 全体の構築: $O(N^3)$ 時間、$O(N + K)$ 追加領域。単純多角形では $K = O(N)$ である。

## 注意点

- 頂点数が 3 未満、隣接する頂点が同一点、隣接する 3 頂点が一直線上、自己交差、面積 0、非有限な座標の場合は `std::invalid_argument` を送出する。穴は扱えない。
- 座標と event 時刻は `long double` で計算するため、極端に近接した event の分類は浮動小数点の精度に依存する。内部の整合性検査に失敗した場合は `std::runtime_error` 等を送出することがある。
- 一般位置は要求しない。矩形のように edge event が同時に起きる場合、複数の split が同時に起きる凹多角形、平行辺が線分に潰れる場合も扱う。

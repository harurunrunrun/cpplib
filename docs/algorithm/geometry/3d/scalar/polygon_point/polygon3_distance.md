---
title: Polygon3 Point Distance (三次元多角形と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polygon_point/polygon3_distance.hpp
---

## API

- `polygon3_distance(polygon, point)`: 閉じた多角形領域と点の最短距離を返す。点を多角形の平面へ射影した点が領域内なら平面との距離を、領域外なら全境界辺との距離の最小値を返す。凹多角形にも対応する。

## API別の時間計算量・空間計算量

- `polygon3_distance(polygon, point)`: 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(N)$。

## 注意点

- `polygon` は頂点数 3 以上の単純な共面多角形で、面積が正でなければならない。頂点数不足、面積 $0$、非共面の場合は `std::invalid_argument` を送出する。
- 全頂点と `point` は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 先頭頂点を基準点とし、頂点との差から求めた局所スケールで多角形とクエリ点を正規化する。絶対座標の大きさを局所スケールに混ぜないため、巨大な共通平行移動がある場合も表現可能な局所的な辺を失わない。
- 面を定める 2 方向は、基準点からの最初の非零方向を 1 回の走査で選び、それと非平行な方向を続く 1 回の走査で選ぶ。長い共線な先頭部分や面積 $0$ の入力でも、この判定は $O(N)$ で済む。全頂点の共面性は適応的述語（`adaptive_orient3d`）で検査する。
- クエリ点との差を局所スケールで表現できない場合や、真の距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
- 自己交差の検査は行わない。

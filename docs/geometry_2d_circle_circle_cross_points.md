---
title: 2D Circle-Circle Cross Points (二次元円同士の交点) [CGL_7_E]
documentation_of: ../src/algorithm/geometry/2d/circle_circle_cross_points.hpp
---

## API

- `circle_circle_cross_points(first, second)`: 2円周の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

一致する円は交点が無限にあるため `std::domain_error`、負半径には `std::invalid_argument` を送出する。接点は1個だけ返す。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 同一点の半径0円同士は交点1個を返す。std::domain_errorとなるのは一致する正半径円である。

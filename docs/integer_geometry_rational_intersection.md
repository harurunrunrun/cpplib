---
title: Rational Line and Segment Intersection (有理数直線・線分交点)
documentation_of: ../src/algorithm/integer_geometry/rational_intersection.hpp
---

## API

- `rational_point(point)`: 整数点を分母 $1$ の `RationalPoint` に変換する。
- `line_intersection(a, b)`: 二つの `Line` の一意な交点を既約な有理点で返す。平行、同一直線、いずれかが退化している場合は `std::nullopt`。
- `segment_intersection_point(a, b)`: 二閉線分の交点がちょうど一つなら既約な有理点で返す。非交差または正の長さで重なる場合は `std::nullopt`。

## API別の時間計算量・空間計算量

各APIはユークリッドの互除法を含めて時間 $O(\log V)$、追加領域 $O(1)$。ここで $V$ は分子・分母の絶対値である。

## 注意点

分母は常に正で、各座標を独立に既約化する。浮動小数点への変換は行わない。直線の媒介変数を先に既約化してから座標を構成するため、未約分の巨大な分子・分母による不要なoverflowを避ける。それでも有理交点を構成する積・和または正規化結果が符号付き `__int128_t` で表せない場合は `std::overflow_error` を送出する。

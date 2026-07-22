---
title: PlaneArrangement3 (三次元平面アレンジメント結果)
documentation_of: ../src/algorithm/geometry/3d/shape/plane_arrangement_result3.hpp
---

平面アレンジメントの重複除去済み平面、交線、三面交点、三次元領域数を保持する。

## API

- `planes`: 幾何的に一致する平面を一つにした列。
- `intersection_lines`: 2平面が定める相異なる交線。
- `intersection_points`: 3平面が定める相異なる交点。
- `region_count`: 平面群が三次元空間を分割する連結領域数。

## API別の時間計算量・空間計算量

各memberへの参照は $O(1)$、列の走査は要素数に比例する。格納領域は平面数 $P$、
相異なる交線数 $L$、交点数 $Q$ として $O(P+L+Q)$。

## 注意点

- `region_count` は境界上のlower-dimensional cell数ではなく三次元領域数である。
- 交線は方向や基準点が異なっても幾何的に一致すれば一つにまとめる。

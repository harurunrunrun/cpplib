---
title: 2D Intersection Test (二次元交差判定) [CGL_2_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/intersect.hpp
---

線分どうし、および直線と点の交差判定の API をまとめた集約ヘッダである。

## API

- `intersect(first_segment, second_segment)`: 2 つの閉線分が共有点を持つかどうかを判定する。
- `intersect(line, point)`: 点が無限直線上にあるかどうかを判定する。

## 時間計算量・空間計算量

各オーバーロードとも、時間 $O(1)$、追加の空間 $O(1)$ である。

## 注意点

- 線分版は、端点での接触、重なり、退化した線分を含めて判定する。各向き判定には、基準となる線分からの垂直距離に基づく許容誤差を用いる。
- 直線と点の版は、退化した直線に対して `std::invalid_argument` を送出する。
- 退化と判定されない範囲では、直線の方向ベクトルを正の定数倍しても、点が直線上にあるかどうかの判定は変わらない。

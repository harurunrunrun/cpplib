---
title: 2D Parallelism (二次元平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/parallel.hpp
---

ベクトルどうし、直線どうしの平行判定の API をまとめた集約ヘッダである。

## API

- `parallel(left, right)`: 2 つのベクトルが平行かどうかを判定する。
- `parallel(left_line, right_line)`: 2 直線の方向ベクトルが平行かどうかを判定する。

## 計算量

各オーバーロードとも、時間 $O(1)$、追加の空間 $O(1)$ である。

## 注意点

- 2 つのベクトルの長さの積をスケールとする、許容誤差付きの外積の判定を行う。そのため、各ベクトルを非零の定数倍しても判定は変わらない。
- ベクトル版は、零ベクトルを含む場合に従来との互換性のため `true` を返す。
- 直線版は、退化した直線に対して `std::invalid_argument` を送出する。

---
title: 2D Orthogonality (二次元直交判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/orthogonal.hpp
---

ベクトルどうし、直線どうしの直交判定の API をまとめた集約ヘッダである。

## API

- `orthogonal(left, right)`: 2 つのベクトルが直交するかどうかを判定する。
- `orthogonal(left_line, right_line)`: 2 直線の方向ベクトルが直交するかどうかを判定する。

## 計算量

各オーバーロードとも、時間 $O(1)$、追加の空間 $O(1)$ である。

## 注意点

- 2 つのベクトルの長さの積をスケールとする、許容誤差付きの内積の判定を行う。そのため、各ベクトルを非零の定数倍しても判定は変わらない。
- ベクトル版は、零ベクトルを含む場合に従来との互換性のため `true` を返す。
- 直線版は、退化した直線に対して `std::invalid_argument` を送出する。

---
title: Spherical SLERP (球面線形補間)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/spherical_slerp.hpp
---

## API

- `spherical_slerp(first, second, parameter)`: 2 方向を結ぶ短い方の大円弧を `parameter` で補間した単位ベクトルを返す。`parameter` が $0$ なら `first` の方向、$1$ なら `second` の方向になる。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 入力は有限な非零ベクトル、`parameter` は区間 $[0, 1]$ 内の有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 2 方向が正反対の場合は経路が一意でないため `std::domain_error` を送出する。
- 2 方向のなす角が非常に小さい場合は、線形補間を正規化した値を返す。
- 入力ベクトルの長さは結果に影響しない。

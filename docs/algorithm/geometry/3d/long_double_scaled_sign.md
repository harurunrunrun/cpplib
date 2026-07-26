---
title: Scaled Long Double Sign (倍率付きlong double符号判定)
documentation_of: ../../../../src/algorithm/geometry/3d/long_double_scaled_sign.hpp
---

## API

- `geometry3d_scaled_sign(value, scale)` は `GEOMETRY3D_EPS * scale` を許容幅として -1、0、1 を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

`value` と `scale` は有限、`scale` は非負でなければならない。違反時は `std::invalid_argument` を送出する。

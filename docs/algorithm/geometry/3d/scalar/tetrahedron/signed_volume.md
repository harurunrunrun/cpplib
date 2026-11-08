---
title: Tetrahedron Signed Volume (四面体の符号付き体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/tetrahedron/signed_volume.hpp
---

## API

- `signed_volume(tetrahedron)`: 頂点 $a, b, c, d$ の順序に対応する符号付き体積 $((b - a) \times (c - a)) \cdot (d - a) / 6$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 符号は頂点の順序と右手系に依存する。$d$ が $(b - a) \times (c - a)$ の向きの側にあれば正である。
- 4 点が同一平面上にある退化四面体では $0$ を返す（浮動小数点演算で直接計算するため、誤差により厳密に $0$ になるとは限らない）。
- 入力の検証やオーバーフロー対策は行わない。座標が巨大な場合、結果が非有限値になりうる。

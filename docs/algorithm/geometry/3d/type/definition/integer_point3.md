---
title: Integer Point 3D (三次元整数点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/integer_point3.hpp
---

## 公開型

- `IntegerPoint3`: `std::array<std::int64_t, 3>` の別名。各要素を $x,y,z$ 座標として扱う。

## API

関数APIは持たず、整数座標を受け取る三次元幾何predicateの共通型を提供する。

## API別の時間計算量・空間計算量

構築・コピー・添字アクセスはいずれも時間・追加領域 $O(1)$。

## 注意点

座標演算そのものは提供しない。算術時は `std::int64_t` の差や積を直接計算せず、対応する正確判定APIを利用する。

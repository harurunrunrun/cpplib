---
title: Euclidean MST Edge (ユークリッド最小全域木の辺)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/euclidean_mst_edge.hpp
---

整数座標点のユークリッド最小全域木を構成する1辺。

## API

```cpp
struct EuclideanMstEdge {
    std::size_t from;
    std::size_t to;
    __uint128_t squared_distance;
};
```

- `from`, `to`: 元の入力点列に対する両端の添字。
- `squared_distance`: 両端間のユークリッド距離の二乗。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 構築、各fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

`squared_distance` は非負の128-bit符号なし整数である。標準出力へ直接流す
overloadは標準ライブラリにないため、必要なら10進変換を実装する。

## 使用例

```cpp
const EuclideanMstEdge edge{0, 1, static_cast<__uint128_t>(25)};
```

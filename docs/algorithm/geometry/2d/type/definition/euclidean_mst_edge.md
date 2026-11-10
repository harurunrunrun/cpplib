---
title: Euclidean MST Edge (ユークリッド最小全域木の辺)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/euclidean_mst_edge.hpp
---

整数座標点のユークリッド最小全域木を構成する 1 本の辺。

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
| 構築、各フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

`squared_distance` は非負の 128 bit 符号なし整数である。標準ライブラリには `__uint128_t` を
出力ストリームへ直接書き出すオーバーロードがないため、必要なら 10 進数への変換を自分で実装する。

## 使用例

```cpp
const EuclideanMstEdge edge{0, 1, static_cast<__uint128_t>(25)};
```

---
title: Pick Theorem Result (ピックの定理の結果)
documentation_of: ../../../../src/algorithm/geometry/2d/pick_theorem_result.hpp
---

単純格子多角形の境界上と内部の格子点数を保持する結果型。

## API

```cpp
struct PickTheoremResult {
    __uint128_t boundary_points;
    __uint128_t interior_points;
};
```

- `boundary_points`: 多角形の境界上にある相異なる格子点数。
- `interior_points`: 多角形の内部にある格子点数。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| aggregate構築、コピー、各fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

両fieldは非負の128-bit符号なし整数である。既定初期化だけでは値が初期化されないため、
値初期化または `pick_theorem` の戻り値を使う。

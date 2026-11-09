---
title: Pick Theorem Result (ピックの定理の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/pick_theorem_result.hpp
---

単純格子多角形の境界上と内部にある格子点の個数を保持する結果型。

## API

```cpp
struct PickTheoremResult {
    __uint128_t boundary_points;
    __uint128_t interior_points;
};
```

- `boundary_points`: 多角形の境界上にある相異なる格子点の個数。
- `interior_points`: 多角形の内部にある格子点の個数。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 集成体としての構築、コピー、各フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

両フィールドは非負の 128 bit 符号なし整数である。デフォルト初期化だけでは値が初期化されないため、
値初期化（`PickTheoremResult{}`）するか、`pick_theorem` の戻り値を使う。

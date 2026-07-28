---
title: Counterclockwise Result (反時計回り判定の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/ccw_result.hpp
---

3点の向きと共線時の位置を表す非scoped列挙型。

## API

```cpp
enum CcwResult {
    CLOCKWISE = -1,
    COUNTER_CLOCKWISE = 1,
    ONLINE_BACK = 2,
    ONLINE_FRONT = -2,
    ON_SEGMENT = 0,
};
```

各値は既存の整数出力規約と一致する。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 列挙値の構築・比較・整数変換 | $O(1)$ | $O(1)$ |

## 注意点

後方互換性のためunscoped enumであり、列挙子はglobal namespaceへ導入される。
数値はAPI契約の一部であり変更しない。

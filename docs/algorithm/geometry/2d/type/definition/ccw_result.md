---
title: Counterclockwise Result (反時計回り判定の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/ccw_result.hpp
---

3 点の向きと、共線のときの位置関係を表す unscoped 列挙型。

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

各値は、CCW 判定で広く使われる整数値の規約に従う。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 列挙値の構築・比較・整数への変換 | $O(1)$ | $O(1)$ |

## 注意点

後方互換性のため unscoped 列挙型であり、列挙子はグローバル名前空間に導入される。
数値は API の契約の一部であり、変更しない。

---
title: Circle Relation Result (円の位置関係の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/circle_relation_result.hpp
---

2円の位置関係を表す非scoped列挙型。

## API

```cpp
enum CircleRelation {
    CIRCLE_INSIDE = 0,
    CIRCLE_INTERNALLY_TANGENT = 1,
    CIRCLE_INTERSECT = 2,
    CIRCLE_EXTERNALLY_TANGENT = 3,
    CIRCLE_SEPARATE = 4,
    CIRCLE_COINCIDENT = 5,
};
```

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 列挙値の構築・比較・整数変換 | $O(1)$ | $O(1)$ |

## 注意点

後方互換性のためunscoped enumであり、列挙子はglobal namespaceへ導入される。
数値はAPI契約の一部であり変更しない。

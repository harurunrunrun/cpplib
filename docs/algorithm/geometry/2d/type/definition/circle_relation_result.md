---
title: Circle Relation Result (円の位置関係の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/circle_relation_result.hpp
---

2 円の位置関係を表す unscoped 列挙型。

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

値 $0$ から $4$ は、共通接線の本数（内包・内接・交差・外接・離れている）に一致する。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 列挙値の構築・比較・整数への変換 | $O(1)$ | $O(1)$ |

## 注意点

後方互換性のため unscoped 列挙型であり、列挙子はグローバル名前空間に導入される。
数値は API の契約の一部であり、変更しない。

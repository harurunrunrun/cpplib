---
title: Line Direction Relation Result (直線方向関係の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/line_direction_relation_result.hpp
---

2 直線の方向の関係を表す scoped 列挙型。

## API

```cpp
enum class LineDirectionRelation : int {
    NEITHER = 0,
    ORTHOGONAL = 1,
    PARALLEL = 2,
};
```

`NEITHER` は平行でも直交でもない場合、`ORTHOGONAL` は直交、
`PARALLEL` は平行を表す。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築・比較・整数への変換 | $O(1)$ | $O(1)$ |

## 注意点

列挙子の整数値は API の契約の一部である。

---
title: 2D Circle Relation Aggregator (二次元円の位置関係集約ヘッダ) [CGL_7_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/circle_relation.hpp
---

円どうしの位置関係を表す列挙値と、2 円を入力とする判定 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `circle_relation_result.hpp` | `CircleRelation` と 6 つの列挙値 |
| `circle_relation_circles.hpp` | `circle_relation(first, second)` |

## 集約される API

```cpp
int circle_relation(const Circle& first, const Circle& second);
```

2 円の位置関係を次の整数値で返す。

| 値 | 列挙子 | 意味 |
| --- | --- | --- |
| $0$ | `CIRCLE_INSIDE` | 一方が他方の内部に含まれる |
| $1$ | `CIRCLE_INTERNALLY_TANGENT` | 内接する |
| $2$ | `CIRCLE_INTERSECT` | 2 点で交わる |
| $3$ | `CIRCLE_EXTERNALLY_TANGENT` | 外接する |
| $4$ | `CIRCLE_SEPARATE` | 離れている |
| $5$ | `CIRCLE_COINCIDENT` | 一致する |

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `circle_relation`、列挙値の参照 | $O(1)$ | $O(1)$ |

## 注意点

- 非有限の座標・非有限の半径・負の半径には `std::invalid_argument` を送出する。
- 接触と一致は、中心間距離・半径の和・半径の差を同じ長さのスケールで比較して判定する。

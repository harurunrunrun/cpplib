---
title: 2D Circle Relation Aggregator (二次元円の位置関係集約ヘッダ) [CGL_7_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/circle_relation.hpp
---

円同士の位置関係列挙値と2円入力APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `circle_relation_result.hpp` | `CircleRelation` と6列挙値 |
| `circle_relation_circles.hpp` | `circle_relation(first, second)` |

## 集約されるAPI

```cpp
int circle_relation(const Circle& first, const Circle& second);
```

包含、内接、2点交差、外接、分離、一致のいずれかを整数値で返す。

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `circle_relation`、列挙値参照 | $O(1)$ | $O(1)$ |

## 注意点

非有限座標・非有限半径・負半径には `std::invalid_argument` を送出する。
接触と一致は中心距離・半径和・半径差を同じ長さscaleで比較する。

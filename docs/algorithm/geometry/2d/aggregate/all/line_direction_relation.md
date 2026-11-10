---
title: 2D Line Direction Relation Aggregator (二次元直線方向関係集約ヘッダ) [CGL_2_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/line_direction_relation.hpp
---

2 直線の方向の関係を表す結果型と、2 直線を入力とする API をまとめた後方互換の集約ヘッダである。既存のコードはこのヘッダをそのまま利用できる。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `line_direction_relation_result.hpp` | `LineDirectionRelation` |
| `line_direction_relation_lines.hpp` | `line_direction_relation(first, second)` |

## 集約される API

```cpp
enum class LineDirectionRelation : int {
    NEITHER = 0,
    ORTHOGONAL = 1,
    PARALLEL = 2,
};

LineDirectionRelation line_direction_relation(const Line& first, const Line& second);
```

2 直線が直交するなら `ORTHOGONAL`、平行（一致を含む）なら `PARALLEL`、どちらでもなければ `NEITHER` を返す。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `line_direction_relation`、結果値の参照 | $O(1)$ | $O(1)$ |

## 注意点

どちらかの直線が退化している場合は `std::invalid_argument` を送出する。

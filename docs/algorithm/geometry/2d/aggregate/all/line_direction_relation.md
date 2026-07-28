---
title: 2D Line Direction Relation Aggregator (二次元直線方向関係集約ヘッダ) [CGL_2_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/line_direction_relation.hpp
---

直線方向関係の結果型と2直線入力APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `line_direction_relation_result.hpp` | `LineDirectionRelation` |
| `line_direction_relation_lines.hpp` | `line_direction_relation(first, second)` |

## 集約されるAPI

```cpp
LineDirectionRelation line_direction_relation(
    const Line& first,
    const Line& second
);
```

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `line_direction_relation`、結果値の参照 | $O(1)$ | $O(1)$ |

## 注意点

どちらかの直線が退化している場合は `std::invalid_argument` を送出する。
既存コードはこのヘッダを変更せず利用できる。

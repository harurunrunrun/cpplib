---
title: Line Direction Relation for Two Lines (2直線の方向関係)
documentation_of: ../../../../src/algorithm/geometry/2d/line_direction_relation_lines.hpp
---

2本の直線が平行、直交、またはそのどちらでもないかを分類する。

## API

```cpp
LineDirectionRelation line_direction_relation(
    const Line& first,
    const Line& second
);
```

## 引数・戻り値

`first` と `second` はそれぞれ異なる2点で定めた直線。
`LineDirectionRelation::PARALLEL`、`ORTHOGONAL`、`NEITHER` のいずれかを返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `line_direction_relation(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

許容誤差付きの外積・内積判定を用い、平行を先に判定する。
いずれかの直線が退化している場合は `std::invalid_argument` を送出する。

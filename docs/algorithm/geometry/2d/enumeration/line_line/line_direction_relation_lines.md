---
title: Line Direction Relation for Two Lines (2 直線の方向関係)
documentation_of: ../../../../../../src/algorithm/geometry/2d/enumeration/line_line/line_direction_relation_lines.hpp
---

2 本の直線が平行か、直交か、そのどちらでもないかを分類する。

## API

```cpp
LineDirectionRelation line_direction_relation(
    const Line& first,
    const Line& second
);
```

## 引数・戻り値

`first` と `second` は、それぞれ相異なる 2 点で定めた直線である。
`LineDirectionRelation::PARALLEL`、`ORTHOGONAL`、`NEITHER` のいずれかを返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `line_direction_relation(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

- 許容誤差付きの外積・内積で判定し、平行かどうかを先に判定する。許容幅は 2 本の方向ベクトルの長さの積に `GEOMETRY_EPS` を掛けたものと丸め誤差の和である。
- どちらかの直線が退化している（方向ベクトルの長さが `GEOMETRY_EPS` 以下の）場合は `std::invalid_argument` を送出する。

---
title: Maximum Manhattan Threshold Matching (Manhattan距離閾値最大マッチング) [TAXI]
documentation_of: ../src/algorithm/matching/maximum_manhattan_threshold_matching.hpp
---

2つの二次元点集合について、Manhattan距離が指定値以下の点同士だけを対応させる二部マッチングの最大サイズを返す。

## `maximum_manhattan_threshold_matching`

```cpp
int maximum_manhattan_threshold_matching(
    const vector<pair<long long, long long>>& left_points,
    const vector<pair<long long, long long>>& right_points,
    long long maximum_distance
);
```

左集合の点 `left_points[i]` と右集合の点 `right_points[j]` の間に、
$|x_i-x_j|+|y_i-y_j|\leq maximum\_distance$ のときだけ辺を張り、互いに端点を共有しない辺の最大本数を返す。
座標の単位と `maximum_distance` の単位は呼び出し側でそろえる。

## API別の時間計算量・空間計算量

$L=left\_points.size()$、$R=right\_points.size()$、距離条件を満たす組数を $E$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_manhattan_threshold_matching` | $O(LR+E\sqrt{L+R})$ | $O(L+R+E)$ |

## 注意点

- `maximum_distance` は非負でなければならず、違反時は `invalid_argument` を送出する。
- 各点集合の要素数は `int` に収まる必要があり、違反時は `length_error` を送出する。
- 座標差と距離の計算には符号付き128 bit整数を用いるため、`long long` の端の値を含む座標でも差のoverflowを起こさない。

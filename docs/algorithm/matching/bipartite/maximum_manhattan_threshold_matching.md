---
title: Maximum Manhattan Threshold Matching (Manhattan距離閾値最大マッチング) [TAXI]
documentation_of: ../../../../src/algorithm/matching/bipartite/maximum_manhattan_threshold_matching.hpp
---

2 つの二次元点集合について、マンハッタン距離が指定値以下の点同士だけを対応させる二部マッチングの最大サイズを返す。

## `maximum_manhattan_threshold_matching`

```cpp
int maximum_manhattan_threshold_matching(
    const vector<pair<long long, long long>>& left_points,
    const vector<pair<long long, long long>>& right_points,
    long long maximum_distance
);
```

`maximum_distance` を $D$ とする。左集合の点 `left_points[i]` $= (x_i, y_i)$ と右集合の点 `right_points[j]` $= (x_j, y_j)$ の間に、$\lvert x_i - x_j \rvert + \lvert y_i - y_j \rvert \le D$ のときだけ辺を張り、互いに端点を共有しない辺の最大本数を返す。座標と `maximum_distance` の単位は呼び出し側でそろえる。

## 時間計算量・空間計算量

$L$ を `left_points.size()`、$R$ を `right_points.size()`、距離条件を満たす組の数を $E$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_manhattan_threshold_matching` | $O(LR + E \sqrt{L + R})$ | $O(L + R + E)$ |

## 注意点

- `maximum_distance` は非負でなければならず、違反時は `invalid_argument` を送出する。
- 各点集合の要素数は `int` に収まる必要があり、違反時は `length_error` を送出する。
- 座標差と距離の計算には符号付き 128 ビット整数を用いるため、`long long` の端の値を含む座標でも差がオーバーフローしない。

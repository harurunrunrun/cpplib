---
title: Assignment Problem (割当問題) [assignment]
documentation_of: ../../../../src/algorithm/matching/bipartite/assignment.hpp
---

ハンガリー法により、長方形のコスト行列に対する最小コストの割当を求める。各行に互いに異なる列を 1 つずつ割り当てる。

## API

```cpp
AssignmentResult<T> minimum_cost_assignment(
    const vector<vector<T>>& cost,
    T infinity = numeric_limits<T>::max() / 4
)
```

行数を $R$、列数を $C$ として $R \le C$ を要求する。行列が長方形でない場合、または $R > C$ の場合は `runtime_error` を送出する。負のコストにも対応する。

`infinity` には、コストと最適解の絶対値より十分大きく、演算でオーバーフローしない値を指定する。途中で有限の割当が存在しないと判定された場合（スラックが `infinity` 以上になる場合）も `runtime_error` を送出する。

戻り値は次のメンバを持つ。

```cpp
T cost;
vector<int> row_to_column;
vector<int> column_to_row;
vector<T> row_potential;
vector<T> column_potential;
```

- `cost`: 最小コスト
- `row_to_column[i]`: 行 `i` に割り当てた列
- `column_to_row[j]`: 列 `j` に割り当てた行。未使用なら `-1`
- `row_potential`, `column_potential`: 双対ポテンシャル。すべての $(i, j)$ で `row_potential[i] + column_potential[j] <= cost[i][j]` が成り立ち、割当に使った組では等号が成り立つ

## 時間計算量

- $O(R^2 C)$
- 正方行列（$R = C = N$）では $O(N^3)$

## 空間計算量

- 入力を除いて $O(R + C)$

---
title: Assignment Problem (割当問題) [assignment]
documentation_of: ../src/algorithm/matching/bipartite/assignment.hpp
---

Hungarian法により長方形cost matrixの最小費用assignmentを求める。各rowへ互いに異なるcolumnを1つずつ割り当てる。

## API

```cpp
AssignmentResult<T> minimum_cost_assignment(
    const vector<vector<T>>& cost,
    T infinity = numeric_limits<T>::max() / 4
)
```

row数を$R$、column数を$C$として$R\le C$を要求する。matrixが長方形でない場合、または$R>C$の場合は例外を送出する。負のcostに対応する。

`infinity`はcostと最適解の絶対値より十分大きく、演算でoverflowしない値を指定する。

返り値は次を持つ。

```cpp
T cost;
vector<int> row_to_column;
vector<int> column_to_row;
vector<T> row_potential;
vector<T> column_potential;
```

- `cost`: 最小費用
- `row_to_column[i]`: row `i`へ割り当てたcolumn
- `column_to_row[j]`: column `j`へ割り当てたrow。未使用なら`-1`
- `row_potential`, `column_potential`: dual potential。すべての$(i,j)$で`row_potential[i] + column_potential[j] <= cost[i][j]`、割当辺で等号が成り立つ

## 時間計算量

- $O(R^2C)$
- 正方行列では $O(N^3)$

## 空間計算量

- 入力を除いて $O(R+C)$

## 注意点

辺端点は $[0,N)$ に収める。多重辺、自己loop、完全matchingが存在しない場合の扱いは各APIの説明に従う。

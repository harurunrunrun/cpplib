---
title: Horn SAT (Horn充足可能性)
documentation_of: ../src/algorithm/graph/constraints/horn_sat.hpp
---

各節に正リテラルが高々1個だけ現れるHorn CNFの充足可能性を判定し、
充足可能なら成分ごとに最小のモデルを返す。

## 節の表現

`HornClause{negative_variables, positive_variable}` は

$$
\lnot x_{negative\_variables[0]}\lor\cdots\lor
\lnot x_{negative\_variables[k-1]}\lor x_{positive\_variable}
$$

を表す。
`positive_variable == -1` は正リテラルを持たない節である。
この場合、すべての `negative_variables` が真になることを禁止する。

## 型

### `HornClause`

- `negative_variables`
    - 負リテラルとして現れる変数番号。重複していてもよい。
- `positive_variable`
    - 正リテラルの変数番号。正リテラルがなければ `-1`。

### `HornSatResult`

- `satisfiable`
    - 充足可能なら `true`。
- `minimum_model`
    - 充足可能なときの最小モデル。indexが変数番号で、0が偽、1が真。
      充足不能なら空。

## 関数

### `horn_sat(variable_count, clauses)`

全変数を偽から始め、負リテラル側の変数がすべて真になった節の
正リテラルを真にする前向き連鎖を行う。
正リテラルを持たない節が発火した場合は充足不能。

`variable_count >= 0`、各変数番号は
`[0, variable_count)`、`positive_variable` は `-1` または
同じ範囲でなければならない。違反時は `runtime_error`。

## 使用例

```cpp
std::vector<HornClause> clauses = {
    {.negative_variables = {}, .positive_variable = 0},
    {.negative_variables = {0}, .positive_variable = 1},
    {.negative_variables = {1}, .positive_variable = -1},
};
auto result = horn_sat(2, clauses);
```

## API別の時間計算量・空間計算量

$N$ を変数数、$C$ を節数、$L$ を負リテラルの出現数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `HornClause` のfield参照 | $O(1)$ | $O(1)$ |
| `HornSatResult` のfield参照 | $O(1)$ | $O(1)$ |
| `horn_sat(variable_count,clauses)` | $O(N+C+L)$ | $O(N+C+L)$ |

## 注意点

- 空の正節
  `HornClause{.negative_variables = {}, .positive_variable = v}` は
  $x_v$ を真に固定する。
- リテラルを1個も持たない節
  `HornClause{.negative_variables = {}, .positive_variable = -1}` があれば
  充足不能。
- 最小モデルは、任意の充足モデルで真になる必要がある変数だけを真にする。

---
title: Horn SAT (Horn充足可能性)
documentation_of: ../../../../../src/algorithm/graph/constraints/logical/horn_sat.hpp
---

各節に正リテラルが高々 1 個しか現れない Horn CNF の充足可能性を判定し、充足可能なら成分ごとに最小のモデルを返す。

## 節の表現

`negative_variables` を $a_1, \ldots, a_k$、`positive_variable` を $b$ とすると、`HornClause{negative_variables, positive_variable}` は

$$
\lnot x_{a_1} \lor \cdots \lor \lnot x_{a_k} \lor x_b
$$

を表す。
`positive_variable == -1` は正リテラルを持たない節であり、この場合は `negative_variables` がすべて真になることを禁止する。

## 型

### `HornClause`

- `negative_variables`
    - 負リテラルとして現れる変数番号の列（`std::vector<int>`）。重複していてもよい。
- `positive_variable`
    - 正リテラルの変数番号。正リテラルがなければ $-1$（デフォルト値）。

### `HornSatResult`

- `satisfiable`
    - 充足可能なら `true`。
- `minimum_model`
    - 充足可能なときの最小モデル（`std::vector<char>`）。添字が変数番号で、0 が偽、1 が真。充足不能なら空。

## 関数

### `horn_sat(variable_count, clauses)`

全変数を偽から始め、負リテラル側の変数がすべて真になった節の正リテラルを真にする前向き連鎖を行う。
正リテラルを持たない節の負リテラル側がすべて真になった場合は充足不能とする。

`variable_count` は非負、`negative_variables` の各変数番号は $[0, N)$、`positive_variable` は $-1$ または $[0, N)$ でなければならない（$N$ は `variable_count`）。違反時は `std::runtime_error` を送出する。

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

$N$ を変数の個数、$C$ を節の個数、$L$ を負リテラルの出現回数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `HornClause` のメンバ参照 | $O(1)$ | $O(1)$ |
| `HornSatResult` のメンバ参照 | $O(1)$ | $O(1)$ |
| `horn_sat(variable_count, clauses)` | $O(N + C + L)$ | $O(N + C + L)$ |

## 注意点

- 負リテラルを持たない節 `HornClause{.negative_variables = {}, .positive_variable = v}` は $x_v$ を真に固定する。
- リテラルを 1 個も持たない節 `HornClause{.negative_variables = {}, .positive_variable = -1}` があれば充足不能になる。
- 最小モデルは、任意の充足モデルで真になる必要がある変数だけを真にする。

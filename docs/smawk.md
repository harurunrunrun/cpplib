---
title: SMAWK (SMAWK法)
documentation_of: ../src/algorithm/other/smawk.hpp
---

totally monotoneな行列の各行について、最小値を取る最左の列を求める。行列は保持せず、要素を返す関数だけを受け取る。

## `smawk_row_minima`

```cpp
template<class Value>
vector<int> smawk_row_minima(int row_count, int column_count, Value value)
```

`value(row, column)` を要素とする `row_count` 行 `column_count` 列の行列を対象とする。戻り値の `result[row]` は、その行で最小値を取る最左の列番号。

`value` は同じ引数に同じ値を返し、その値に `<` を適用できる必要がある。行列はtotally monotone、すなわち任意の $i<j$ と $k<l$ について

$$
A_{i,k}>A_{i,l}\Longrightarrow A_{j,k}>A_{j,l}
$$

を満たすことを前提とする。この前提は実行時には検査しない。Monge行列はtotally monotoneである。

`row_count == 0` なら空列を返す。行が存在するのに `column_count == 0` の場合、または負の次元を渡した場合は `invalid_argument`。

## API別の時間計算量・空間計算量

$R=$ `row_count`、$C=$ `column_count` とする。

| API | 時間計算量 | `value` の呼び出し回数 | 空間計算量（追加領域） |
| --- | --- | --- | --- |
| `smawk_row_minima` | $O(R+C)$ | $O(R+C)$ | $O(R+C)$ |

再帰の深さは $O(\log R)$。同値な最小値が複数ある場合も、strict comparisonで既存の小さい列を残すため最左を返す。

## 注意点

空入力、退化入力、範囲外のindexと容量超過の扱いは各APIの説明に従う。参照・iterator・callbackを受け渡すAPIでは、変更可否とobjectの寿命を守る。

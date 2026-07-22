---
title: Parenthesized Expression Extrema (括弧付け式の最大値・最小値) [LISA]
documentation_of: ../src/algorithm/other/dynamic_programming/parenthesized_expression_extrema.hpp
---

非負整数、`+`、`*` からなる式の括弧付けで得られる最小値と最大値を返す。

## ParenthesizedExpressionExtrema

```cpp
struct ParenthesizedExpressionExtrema {
    unsigned long long minimum, maximum;
};
```

## parenthesized_expression_extrema

```cpp
ParenthesizedExpressionExtrema parenthesized_expression_extrema(const string& expression);
```

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `parenthesized_expression_extrema` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 空白や単項演算子は受理しない。
- 構文違反は `runtime_error`、範囲違反は `overflow_error` となる。

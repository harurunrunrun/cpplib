---
title: Parenthesized Expression Extrema (括弧付け式の最大値・最小値) [LISA]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/parenthesized_expression_extrema.hpp
---

非負整数と `+`、`*` からなる式に括弧を自由に付けたときに得られる値の最小値と最大値を、区間 DP で求める。

## ParenthesizedExpressionExtrema

```cpp
struct ParenthesizedExpressionExtrema {
    unsigned long long minimum;
    unsigned long long maximum;
};
```

## parenthesized_expression_extrema

```cpp
ParenthesizedExpressionExtrema parenthesized_expression_extrema(const std::string& expression);
```

## API別の時間計算量・空間計算量

$N$ は式に含まれる数の個数である。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `parenthesized_expression_extrema` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 式は「数 演算子 数 … 数」の形でなければならず、空白や単項演算子は受理しない。
- 構文違反では `std::runtime_error` を送出する。数や途中の計算結果が `unsigned long long` に収まらない場合は `std::overflow_error` を送出する。

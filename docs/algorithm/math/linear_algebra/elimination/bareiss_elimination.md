---
title: Bareiss Fraction-Free Elimination (Bareiss分数なし消去法)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/bareiss_elimination.hpp
---

整数行列を、途中で分数を作らずに消去する（Bareiss の分数なし消去法）。各ステップの除算は数学的に割り切れるため、整数のまま厳密に計算できる。いずれの API も名前空間 `linear_algebra` にある。

## `BareissEliminationResult<Integer>`

```cpp
template<class Integer>
struct BareissEliminationResult {
    std::vector<std::vector<Integer>> upper;
    std::size_t rank = 0;
    int row_swap_sign = 1;
};
```

`upper` は消去後の行列（行階段形）、`rank` は階数、`row_swap_sign` は行交換による符号（$\pm 1$）である。保持する行列の空間計算量は $O(mn)$。

## `bareiss_elimination(matrix)`

```cpp
BareissEliminationResult<Integer> bareiss_elimination(std::vector<std::vector<Integer>> matrix);
```

$m\times n$ 行列を分数なしで消去する。行列は値渡しで受け取り、そのまま `upper` として返す。時間計算量は $O(mn\min(m,n))$ 回の整数演算、追加空間計算量は返り値の行列を除いて $O(1)$。

## `bareiss_determinant(matrix)`

```cpp
Integer bareiss_determinant(std::vector<std::vector<Integer>> matrix);
```

正方行列の行列式を厳密に返す。$0 \times 0$ 行列では $1$ を、階数が落ちる場合は $0$ を返す。時間計算量は $O(n^3)$ 回の整数演算、空間計算量は $O(n^2)$。

## 注意点

- `Integer` には、除算が数学的に割り切れるときに正確な商を返す整域の型を使う。固定長整数では、中間値（2 つの小行列式の積の差）が型の範囲を超えないことが前提である。
- `bareiss_elimination` は行の長さがそろっていない入力に、`bareiss_determinant` は正方でない入力に対して `std::invalid_argument` を送出する。

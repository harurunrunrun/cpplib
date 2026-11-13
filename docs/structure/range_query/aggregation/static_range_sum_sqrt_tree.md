---
title: Static Range Sum Sqrt Tree (静的区間和平方根木) [static_range_sum]
documentation_of: ../../../../src/structure/range_query/aggregation/static_range_sum_sqrt_tree.hpp
---

静的な数列の区間和を Sqrt Tree で求める。

## 型

```cpp
template<class T, int MAX_SIZE = -1>
class StaticRangeSumSqrtTree;
```

`T` は加算・`T(0)`・コピーが使える型とする。`MAX_SIZE` は列の長さの上限であり、省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。

## コンストラクタ

```cpp
StaticRangeSumSqrtTree()
explicit StaticRangeSumSqrtTree(const vector<T>& values)
```

空の構造、または `values` を格納した構造を作る。

## `build`

```cpp
void build(const vector<T>& values)
```

格納する列を `values` で再構築する。

## `size`, `empty`

```cpp
int size() const
bool empty() const
```

列の長さと、列が空かどうかを返す。

## `get`

```cpp
T get(int index) const
```

`values[index]` を返す。

## `sum`

```cpp
T sum(int left, int right) const
```

半開区間 `[left, right)` の和を返す。空区間の和は `T(0)` である。

## `all_sum`

```cpp
T all_sum() const
```

全要素の和を返す。空列の和は `T(0)` である。

## 時間計算量

列の長さを $N$、$L=O(\log\log(N+2))$ とする。

- デフォルトコンストラクタ: $O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2))$
- `vector` を受け取るコンストラクタ: $O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2)+NL)$
- `build`: $O(NL)$
- `size`, `empty`, `get`, `sum`, `all_sum`: $O(1)$

`MAX_SIZE` を省略した場合、構築は $O(NL)$ である。

## 空間計算量

$O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2))$（省略時は $O(NL)$）。

## 注意点

- `values.size() <= MAX_SIZE` が必要である。
- `get` の添字と `sum` の区間は列の範囲内でなければならず、違反時は `std::runtime_error` を送出する。

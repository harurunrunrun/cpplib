---
title: Static Range Sum Sqrt Tree (静的区間和平方根木) [static_range_sum]
documentation_of: ../src/structure/other/static_range_sum_sqrt_tree.hpp
---

静的な数列に対する区間和を Sqrt Tree で求める。

## 型

```cpp
StaticRangeSumSqrtTree<T, MAX_SIZE>
```

`T` は加算、`T(0)`、コピーを使用できる型とする。

## コンストラクタ

```cpp
StaticRangeSumSqrtTree()
StaticRangeSumSqrtTree(const vector<T>& values)
```

空の構造、または `values` を格納した構造を作る。

## `build`

```cpp
void build(const vector<T>& values)
```

格納列を `values` で再構築する。

## `size`, `empty`

```cpp
int size() const
bool empty() const
```

列長と、列が空かどうかを返す。

## `get`

```cpp
T get(int index) const
```

`values[index]` を返す。

## `sum`

```cpp
T sum(int left, int right) const
```

半開区間 `[left, right)` の和を返す。空区間の和は `T(0)`。

## `all_sum`

```cpp
T all_sum() const
```

全要素の和を返す。空列の和は `T(0)`。

## 時間計算量

列長を $N$、$L=O(\log\log(N+2))$ とする。

- default constructor: $O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2))$
- vector constructor: $O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2)+NL)$
- `build`: $O(NL)$
- `size`, `empty`, `get`, `sum`, `all_sum`: $O(1)$

## 空間計算量

$O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2))$。

## 注意点

- `values.size() <= MAX_SIZE` が必要。
- `get` の添字と `sum` の区間は列の範囲内でなければならず、違反時は `runtime_error`。

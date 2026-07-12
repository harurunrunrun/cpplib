---
title: Convex Hull Trick
documentation_of: ../src/structure/convex_hull_trick/convex_hull_trick.hpp
---

直線追加と最小値クエリ。

傾きは単調非増加で追加する。

# テンプレート引数

```cpp
ConvexHullTrick<MAX_LINE>
```

- 保持する直線数の上限 `MAX_LINE`

# add_line

```cpp
void add_line(long long a, long long b)
```

直線 `a x + b` を追加する。

## 時間計算量

- amortized $O(1)$

# query

```cpp
long long query(long long x)
```

追加された直線の `x` における最小値を返す。

## 時間計算量

- $O(\log N)$

# query_monotone_inc

```cpp
long long query_monotone_inc(long long x)
```

`x` が単調非減少に与えられる場合の最小値クエリ。

## 時間計算量

- amortized $O(1)$

# その他

```cpp
int size()
bool empty()
```

## 時間計算量

- $O(1)$

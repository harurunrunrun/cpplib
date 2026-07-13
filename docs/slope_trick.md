---
title: Slope Trick
documentation_of: ../src/structure/convex_hull_trick/slope_trick.hpp
---

整数上の凸区分線形関数を、最小値と左右の傾き変化点で保持する。

```cpp
SlopeTrick<T, MAX_SIZE> slope;
```

`MAX_SIZE` は左heap、右heapそれぞれの要素数の上限である。

# 最小値とargmin

```cpp
T min_f() const
pair<T, T> argmin() const
tuple<T, T, T> get_min() const
```

`min_f` は関数の最小値を返す。`argmin` は最小値を取る閉区間を返す。
左端または右端が無限の場合、それぞれ `numeric_limits<T>::lowest()`、
`numeric_limits<T>::max()` を返す。`get_min` は最小値とargminの両端をまとめて返す。

## 時間計算量

- `min_f`: $O(1)$
- `argmin`: $O(1)$
- `get_min`: $O(1)$

# 関数の加算

```cpp
void add_const(T c)
void add_a_minus_x(T a)
void add_x_minus_a(T a)
void add_abs(T a)
```

現在の関数を $f$ とする。

- `add_const(c)`: $f(x) \gets f(x)+c$
- `add_a_minus_x(a)`: $f(x) \gets f(x)+\max(a-x,0)$
- `add_x_minus_a(a)`: $f(x) \gets f(x)+\max(x-a,0)$
- `add_abs(a)`: $f(x) \gets f(x)+|x-a|$

## 時間計算量

- `add_const`: $O(1)$
- `add_a_minus_x`: $O(\log(N+1))$
- `add_x_minus_a`: $O(\log(N+1))$
- `add_abs`: $O(\log(N+1))$

# 累積最小値

```cpp
void clear_left()
void clear_right()
void prefix_min()
void cumulative_min()
void suffix_min()
```

- `clear_left`, `suffix_min`: $f(x) \gets \min_{x \le y} f(y)$
- `clear_right`, `prefix_min`, `cumulative_min`: $f(x) \gets \min_{y \le x} f(y)$

## 時間計算量

すべて $O(1)$。

# 平行移動

```cpp
void shift(T d)
void shift(T l, T r)
```

- `shift(d)`: $f(x) \gets f(x-d)$
- `shift(l, r)`: $f(x) \gets \min_{x-r \le y \le x-l} f(y)$。$l \le r$ が必要

## 時間計算量

どちらも $O(1)$。

# 一点評価

```cpp
T eval(T x) const
```

$f(x)$ を返す。

## 時間計算量

$O(N)$。

# その他

```cpp
int size() const
bool empty() const
int left_size() const
int right_size() const
void clear()
```

`clear` は定数0の関数へ戻す。

## 時間計算量

すべて $O(1)$。

# 空間計算量

$O(\mathrm{MAX\_SIZE})$。各操作の追加領域は $O(1)$。

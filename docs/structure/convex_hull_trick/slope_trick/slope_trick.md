---
title: Slope Trick (傾きトリック)
documentation_of: ../../../../src/structure/convex_hull_trick/slope_trick/slope_trick.hpp
---

凸な区分線形関数を、最小値と、左右の傾きの変化点（折点）を持つ 2 つのヒープで保持する。

```cpp
SlopeTrick<T, MAX_SIZE = -1> slope;
SlopeTrick<T, MAX_SIZE = -1> slope(c);  // 定数関数 f(x) = c で初期化
```

`MAX_SIZE` は左ヒープ・右ヒープそれぞれの要素数の上限である。省略（`-1`）すると `std::vector` による可変長版になり、必要に応じて領域を伸長する。

## 最小値と argmin

```cpp
T min_f() const
pair<T, T> argmin() const
tuple<T, T, T> get_min() const
```

`min_f` は関数の最小値を返す。`argmin` は最小値を取る閉区間の両端を返す。
左端または右端が無限の場合は、それぞれ `numeric_limits<T>::lowest()`、`numeric_limits<T>::max()` を返す。
`get_min` は最小値と argmin の両端をまとめて返す。

## 関数の加算

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
- `add_abs(a)`: $f(x) \gets f(x)+\lvert x-a \rvert$

## 累積最小値

```cpp
void clear_left()
void clear_right()
void prefix_min()
void cumulative_min()
void suffix_min()
```

- `clear_left`、`suffix_min`: $f(x) \gets \min_{x \le y} f(y)$
- `clear_right`、`prefix_min`、`cumulative_min`: $f(x) \gets \min_{y \le x} f(y)$

## 平行移動

```cpp
void shift(T d)
void shift(T l, T r)
```

- `shift(d)`: $f(x) \gets f(x-d)$
- `shift(l, r)`: $f(x) \gets \min_{x-r \le y \le x-l} f(y)$。$l \le r$ が必要

## 一点評価

```cpp
T eval(T x) const
```

$f(x)$ を返す。ヒープ内の全折点を走査して計算する。

## その他

```cpp
int size() const
bool empty() const
int left_size() const
int right_size() const
void clear()
```

`size` は左右のヒープの要素数の合計、`left_size`、`right_size` はそれぞれの要素数を返す。`clear` は定数 $0$ の関数に戻す。

## 時間計算量

$N$ を保持している折点数とする。

- `min_f`、`argmin`、`get_min`、`add_const`: $O(1)$
- `add_a_minus_x`、`add_x_minus_a`、`add_abs`: $O(\log(N+1))$
- `clear_left`、`clear_right`、`prefix_min`、`cumulative_min`、`suffix_min`: $O(1)$
- `shift`: $O(1)$
- `eval`: $O(N)$
- `size`、`empty`、`left_size`、`right_size`、`clear`: $O(1)$

可変長版でも領域の伸長コストは償却され、上記の計算量を保つ。

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$
- 可変長版: 追加した折点数に比例する
- 各操作の追加領域: $O(1)$

## 注意点

- 表現できるのは凸な区分線形関数である。上記の操作はすべて凸性を保つ。
- 固定長版で、要素を追加する側のヒープが `MAX_SIZE` に達している状態で `add_a_minus_x`、`add_x_minus_a`、`add_abs` を呼ぶと `std::runtime_error` を送出する（`add_abs` は左右どちらかが満杯なら送出する）。
- `shift(l, r)` で $r < l$ の場合は `std::runtime_error` を送出する。
- `T` の加減算・比較と、傾きを掛けた値が表現範囲を超えないことは呼び出し側が保証する。

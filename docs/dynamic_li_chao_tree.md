---
title: Dynamic Li Chao Tree (動的Li Chao木) [line_add_get_min]
documentation_of: ../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp
---

動的 Li Chao Tree。

整数座標の閉区間 `[X_MIN, X_MAX]` 上で直線を追加し、各点での最小値を求める。

# テンプレート引数

```cpp
DynamicLiChaoTree<X_MIN, X_MAX, MAX_NODE>
```

- 座標の下限 `X_MIN`
- 座標の上限 `X_MAX`
- ノード数の上限 `MAX_NODE`

直線追加だけなら、必要なノード数は追加した直線数以下。

# add_line

```cpp
void add_line(long long a, long long b)
void add_line(Line line)
```

直線 `a x + b` を追加する。

## 時間計算量

- $O(\log (X\_MAX - X\_MIN + 1))$

# query

```cpp
long long query(long long x)
```

追加された直線の `x` における最小値を返す。

空の場合は `DynamicLiChaoTree::INF` を返す。非空の場合の値は `INF` 以上でも扱える。

`a * x + b` は `__int128` で評価し、`long long` の範囲外だけを上下限へ丸める。容量例外が発生した場合、既存の木は変化しない。

## 時間計算量

- $O(\log (X\_MAX - X\_MIN + 1))$

# その他

```cpp
bool empty()
int nodes_used()
```

## 時間計算量

- $O(1)$

# API別計算量

$W=X\_MAX-X\_MIN+1$、$D=\lceil\log_2 W\rceil+1$ を整数区間treeの最大level数とする。

- implicit default constructor, `nodes_used`, `empty`: $O(1)$
- `Line::eval128`, `Line::eval`: $O(1)$
- 2つの `add_line` overload: $O(D)$。1回の追加で新規nodeは高々1個
- `query`: $O(D)$

## 空間計算量

- 固定arena: $O(\mathrm{MAX\_NODE})$
- 各操作の再帰stack: $O(D)$

## 注意点

- compile時に`X_MIN <= X_MAX`, `MAX_NODE > 0`が必要。
- `query(x)`は`X_MIN <= x <= X_MAX`を要求し、範囲外なら例外。
- node容量を超える`add_line`は例外で、既存treeと使用node数を変えない。
- `nodes_used`は使用node数、`empty`は直線が0本かを返す。

---
title: Dynamic Li Chao Tree
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

## 時間計算量

- $O(\log (X\_MAX - X\_MIN + 1))$

# その他

```cpp
bool empty()
int nodes_used()
```

## 時間計算量

- $O(1)$

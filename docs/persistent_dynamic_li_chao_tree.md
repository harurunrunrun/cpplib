---
title: Persistent Dynamic Li Chao Tree
documentation_of: ../src/structure/convex_hull_trick/persistent_dynamic_li_chao_tree.hpp
---

完全永続 Dynamic Li Chao Tree。

整数座標の閉区間 `[X_MIN, X_MAX]` 上で、指定したバージョンに直線を追加した新しいバージョンを作る。

# テンプレート引数

```cpp
PersistentDynamicLiChaoTree<X_MIN, X_MAX, MAX_VERSION>
```

- 座標の下限 `X_MIN`
- 座標の上限 `X_MAX`
- 追加できるバージョン数の上限 `MAX_VERSION`

# add_line / fork

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
int fork(int version)
```

`add_line` は `version` に直線 `a x + b` を追加した新しいバージョン番号を返す。
`fork` は同じ状態を指す新しいバージョンを作る。

## 時間計算量

- `add_line`: $O(\log (X\_MAX - X\_MIN + 1))$
- `fork`: $O(1)$

# query

```cpp
long long query(int version, long long x)
```

指定したバージョンでの `x` における最小値を返す。

## 時間計算量

- $O(\log (X\_MAX - X\_MIN + 1))$

# その他

```cpp
bool empty(int version)
int versions()
int latest_version()
int nodes_used()
```

## 時間計算量

- $O(1)$

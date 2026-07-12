---
title: Persistent Convex Hull Trick
documentation_of: ../src/structure/convex_hull_trick/persistent_convex_hull_trick.hpp
---

直線追加永続 Convex Hull Trick。最小値を返す。

追加する直線の傾きは単調非増加である必要がある。

# テンプレート引数

```cpp
PersistentConvexHullTrick<MAX_LINE, MAX_VERSION, MAX_NODE>
```

- hull に保持する直線数の上限 `MAX_LINE`
- version 数の上限 `MAX_VERSION`
- 永続セグメント木のノード数上限 `MAX_NODE`

# version

```cpp
int fork(int version)
int versions()
```

`fork` は同じ hull を持つ新しい version を作る。

# add_line

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
```

version に直線 $y=ax+b$ を追加した新しい version を返す。

## 時間計算量

- 削除される直線数を $K$ として $O(K \log N)$

# query

```cpp
long long query(int version, long long x)
```

version の hull での最小値を返す。

## 時間計算量

- $O(\log^2 N)$

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

- 削除される直線数を $K$ として $O((K+1) \log(\mathrm{MAX\_LINE}+1))$

# query

```cpp
long long query(int version, long long x)
```

version の hull での最小値を返す。

`a * x + b` は `__int128` で評価し、`long long` の範囲外だけを上下限へ丸める。

line、node、version の容量例外が発生した場合、既存 version と使用済み node 数は変化しない。

## 時間計算量

- $O(\log(N+1)\log(\mathrm{MAX\_LINE}+1))$

# API別計算量

$N$ を対象versionのhullの直線数、$K$ を `add_line` で末尾から除去する直線数とする。

- implicit default constructor: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `Line::eval128`, `Line::eval`: $O(1)$
- `versions`, `nodes_used`, `size`, `empty`, `fork`: $O(1)$。`fork` はnodeを増やさずversionだけを生成する
- `line_at`: $O(\log(\mathrm{MAX\_LINE}+1))$
- 2つの `add_line` overload: worst-case $O((K+1)\log(\mathrm{MAX\_LINE}+1))$。保存する場合は $O(\log(\mathrm{MAX\_LINE}+1))$ nodeと1 versionを生成する
- `query`: $O(\log(N+1)\log(\mathrm{MAX\_LINE}+1))$

`query` はhull indexを二分探索し、各比較で永続segment treeから直線を取得する。古いversionから分岐すると同じ直線を何度も除去できるため、全分岐を通じた $K$ の償却はできない。直前versionだけを更新する一本道では `add_line` は償却 $O(\log(\mathrm{MAX\_LINE}+1))$。

## 空間計算量

- 固定arena: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- 保存される直線追加ごとの新規node: $O(\log(\mathrm{MAX\_LINE}+1))$

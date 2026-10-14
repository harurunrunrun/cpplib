---
title: Persistent Convex Hull Trick (完全永続凸包トリック)
documentation_of: ../../../../src/structure/convex_hull_trick/line_container/persistent_convex_hull_trick.hpp
---

直線追加永続 Convex Hull Trick。最小値を返す。

追加する直線の傾きは単調非増加である必要がある。

## テンプレート引数

```cpp
PersistentConvexHullTrick<MAX_LINE, MAX_VERSION, MAX_NODE>
```

- hull に保持する直線数の上限 `MAX_LINE`
- version 数の上限 `MAX_VERSION`
- 永続セグメント木のノード数上限 `MAX_NODE`

## version

```cpp
int fork(int version)
int versions()
```

`fork` は同じ hull を持つ新しい version を作る。

## add_line

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
```

version に直線 $y=ax+b$ を追加した新しい version を返す。

## 時間計算量

- 削除される直線数を $K$ として $O((K+1) \log(\mathrm{MAX\_LINE}+1))$

## query

```cpp
long long query(int version, long long x)
```

version の hull での最小値を返す。

`a * x + b` は `__int128` で評価し、`long long` の範囲外だけを上下限へ丸める。

line、node、version の容量例外が発生した場合、既存 version と使用済み node 数は変化しない。

## 時間計算量

- $O(\log(N+1)\log(\mathrm{MAX\_LINE}+1))$

## API別計算量

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

## 状態参照

```cpp
int nodes_used() const
int size(int version) const
bool empty(int version) const
Line line_at(int version, int index) const
```

`nodes_used`は永続arenaの使用node数、`size`はhull直線数、
`empty`はそのversionが空か、`line_at`はhull順の直線を返す。

versionは`[0,versions())`、`line_at`のindexは`[0,size(version))`。
範囲外、傾き単調性違反、line/node/version容量超過は例外。
空versionへの`query`も例外。失敗したupdateは既存versionとnode数を変えない。

## 注意点

各版で追加する傾きは、その版の最後の傾き以下にする。
空versionへのquery、無効なversionやindex、固定版の容量超過は例外。
失敗した更新は既存versionと使用済みノード数を変えない。

## 容量を省略した可変長版

容量テンプレートを省略すると既定値 `-1` でvectorを使い、必要な保存領域を明示的に伸長する。
固定容量を指定する既存版も同じヘッダに残る。

```cpp
using Variable = PersistentConvexHullTrick<>;
using Fixed = PersistentConvexHullTrick<Lines, Versions, Nodes>;
```

各版の索引木は必要なときだけ範囲を倍増し、古い根を左部分木として新しい根を作る。過去版は変更しない。
祖先版を含む最大hull長を $L$、今回除去する直線数を $K$ とすると、追加 $O((K+1)\log(L+1))$、照会 $O(\log(N+1)\log(L+1))$。
領域は作成済みノード・版の数に比例する。分岐を通じて $K$ を一律償却することはできない。
整数添字の表現上限を超える要素数は例外。構造上の文字種類・bit幅・座標域は、件数容量とは別に指定できる。

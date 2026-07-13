---
title: Tree Euler Tour (木のオイラーツアー)
documentation_of: ../src/algorithm/tree/euler_tour.hpp
---

fixed-capacityの根付き木Euler Tour。再帰を使わず、構築時に
`parent`、`depth`、preorder、subtree区間、entry/exit列を同時に求める。

## 型

```cpp
TreeEulerTour<MAX_SIZE> tour(n);
```

実頂点数 `n` は $0\le n\le MAX_SIZE$。
objectは辺・隣接列・構築結果・DFS stackを固定長配列で保持し、動的確保しない。
copy/moveすると固定容量全体をコピーする。

```cpp
struct Event {
    int vertex;
    bool entering;
};
```

`entering=true` が頂点へ入るevent、`false` が出るevent。
各field参照は $O(1)$。

## 辺追加と構築

```cpp
tour.add_edge(u, v);
tour.build(root);
```

無向辺をちょうど $n-1$ 本追加してから `build` する。
連結でない、cycle・self-loop・多重辺を含む、辺数が違う場合は例外。
`n=0` では辺なしの `build()` が成功し、`root() == -1`。

各頂点の隣接頂点は、その頂点に関する `add_edge` の呼出し順でDFSされる。
したがってpreorderとentry/exit列もこの順で決定する。
辺追加後は再度 `build` するまで構築済みqueryを呼べない。

## metadataと頂点query

```cpp
tour.size()
tour.edge_count()
tour.root()
tour.parent(v)
tour.depth(v)
tour.tin(v)
tour.tout(v)
tour.subtree_range(v)
tour.subtree_size(v)
tour.is_ancestor(u, v)
```

`parent(root) == -1`。
頂点 $v$ のsubtreeはpreorder上の半開区間
$[tin(v),tout(v))$ であり、`subtree_size(v) == tout(v)-tin(v)`。
`is_ancestor(u,v)` は同じ頂点をancestorとして扱う。

## preorder

```cpp
tour.preorder_vertex(index)
tour.preorder()
```

`preorder_vertex(i)` はpreorderの $i$ 番目を返す。
`preorder()` は長さ $n$ の列をコピーして返す。

## entry / exit Euler列

```cpp
tour.euler_size()
tour.euler_event(index)
tour.euler_tour()
```

非空木ではentryとexitを各頂点1回ずつ含むため長さは $2n$。
`euler_event(i)` は1event、`euler_tour()` は全列をコピーして返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 |
| --- | --- |
| `TreeEulerTour(n)` | $O(1)$ |
| copy/move構築・代入 | $O(MAX_SIZE)$ |
| `size()` / `edge_count()` | $O(1)$ |
| `add_edge(u,v)` | $O(1)$ |
| `build(root)` | $O(n)$ |
| `root()` / `parent(v)` / `depth(v)` | $O(1)$ |
| `tin(v)` / `tout(v)` / `subtree_range(v)` / `subtree_size(v)` | $O(1)$ |
| `is_ancestor(u,v)` | $O(1)$ |
| `preorder_vertex(i)` | $O(1)$ |
| `preorder()` | $O(n)$、出力 $O(n)$ |
| `euler_size()` / `euler_event(i)` | $O(1)$ |
| `euler_tour()` | $O(n)$、出力 $O(n)$ |

object本体と構築用bufferは $O(MAX_SIZE)$。`build` 中も動的確保はない。

## 注意点

constructorは `0 <= n <= MAX_SIZE`、`add_edge` は固定の `MAX_SIZE-1` 辺容量を検査する。metadata・preorder・event queryは直近の辺追加後のbuild成功と有効indexを必要とし、違反時は `runtime_error`。

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。

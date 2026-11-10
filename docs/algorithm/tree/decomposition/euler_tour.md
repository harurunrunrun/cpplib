---
title: Tree Euler Tour (木のオイラーツアー)
documentation_of: ../../../../src/algorithm/tree/decomposition/euler_tour.hpp
---

根付き木の Euler Tour を求める。
再帰を使わず、構築時に `parent`、`depth`、preorder、部分木の区間、入る・出るイベントの列を同時に求める。

## 型

```cpp
TreeEulerTour<> dynamic(n);          // 可変長版
TreeEulerTour<100000> fixed(n);      // 固定容量版
```

テンプレート引数 `MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。
明示すると、辺・隣接列・構築結果・DFS 用のスタックを固定長配列で保持し、動的確保をしない。固定容量版をコピー・ムーブすると固定容量全体をコピーする。
実際の頂点数 $n$ は $0 \le n$ で、固定容量版では $n \le \texttt{MAX_SIZE}$ でもなければならない。

```cpp
struct Event{
    int vertex;
    bool entering;
};
```

`entering == true` が頂点に入るイベント、`false` が頂点から出るイベントである。
各メンバの参照は $O(1)$ である。

## 辺の追加と構築

```cpp
tour.add_edge(u, v);
tour.build(root);
```

無向辺をちょうど $n-1$ 本追加してから `build` する。
連結でない、閉路・自己ループ・多重辺を含む、辺数が違う場合は例外を送出する。
$n=0$ では辺なしの `build()` が成功し、`root() == -1` となる。

各頂点の隣接頂点は、その頂点に関する `add_edge` の呼び出し順に DFS で訪問される。
したがって、preorder とイベント列もこの順で決まる。
辺を追加した後は、再度 `build` するまで構築済みのクエリを呼べない。

## メタデータと頂点クエリ

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

`parent(root) == -1` である。
頂点 $v$ の部分木は preorder 上の半開区間 $[\mathrm{tin}(v), \mathrm{tout}(v))$ であり、`subtree_size(v) == tout(v) - tin(v)` となる。
`is_ancestor(u, v)` は同じ頂点も祖先として扱う。

## preorder

```cpp
tour.preorder_vertex(index)
tour.preorder()
```

`preorder_vertex(i)` は preorder の $i$ 番目の頂点を返す。
`preorder()` は長さ $n$ の列をコピーして返す。

## 入る・出るイベントの Euler 列

```cpp
tour.euler_size()
tour.euler_event(index)
tour.euler_tour()
```

空でない木では、各頂点について入るイベントと出るイベントを 1 回ずつ含むため、長さは $2n$ である。
`euler_event(i)` は 1 つのイベントを、`euler_tour()` は列全体をコピーして返す。

## API 別の時間計算量・空間計算量

| API | 時間計算量 |
| --- | --- |
| `TreeEulerTour(n)` | 可変長版 $O(n)$、固定容量版 $O(1)$ |
| コピー・ムーブの構築と代入 | 可変長版 $O(n)$、固定容量版 $O(\texttt{MAX_SIZE})$ |
| `size()` / `edge_count()` | $O(1)$ |
| `add_edge(u, v)` | $O(1)$ |
| `build(root)` | $O(n)$ |
| `root()` / `parent(v)` / `depth(v)` | $O(1)$ |
| `tin(v)` / `tout(v)` / `subtree_range(v)` / `subtree_size(v)` | $O(1)$ |
| `is_ancestor(u, v)` | $O(1)$ |
| `preorder_vertex(i)` | $O(1)$ |
| `preorder()` | $O(n)$、出力 $O(n)$ |
| `euler_size()` / `euler_event(i)` | $O(1)$ |
| `euler_tour()` | $O(n)$、出力 $O(n)$ |

オブジェクト本体と構築用のバッファは、可変長版で $O(n)$、固定容量版で $O(\texttt{MAX_SIZE})$ である。固定容量版では `build` 中も動的確保はない。

## 注意点

- コンストラクタは `0 <= n`（固定容量版では `n <= MAX_SIZE` も）を要求する。`add_edge` は辺の容量（可変長版では $n-1$ 本、固定容量版では $\texttt{MAX_SIZE}-1$ 本）を検査する。
- メタデータ・preorder・イベントのクエリは、直近の辺追加の後の `build` の成功と有効な添字を必要とし、違反時は `std::runtime_error` を送出する。
- 頂点と `root` は範囲内でなければならない。空木を除き、入力は無向の連結木とする。端点の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。表のサイズが `int` の範囲を超える場合は `std::length_error` を送出する。

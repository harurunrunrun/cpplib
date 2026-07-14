---
title: DSU on Tree (Sack) (木上のDSU)
documentation_of: ../src/algorithm/tree/dsu_on_tree.hpp
---

各頂点について部分木全体の情報を求める。最大部分木の子をheavy childとして保持し、light childの部分木だけをEuler順に追加し直す。

## 型

```cpp
DsuOnTree<MAX_SIZE> sack(n);
```

実頂点数は $0\leq n\leq MAX_SIZE$。辺、CSR隣接列、Euler順、部分木サイズ、heavy child、処理stackを固定長配列で保持し、`build` と `run` は動的確保も再帰も行わない。

## 辺追加と構築

```cpp
sack.add_edge(u, v);
sack.build(root);
```

無向辺をちょうど $n-1$ 本追加する。`build` は根付き木のpreorderと各部分木の半開区間を構築し、部分木サイズ最大の子をheavy childにする。同率の場合のheavy childはCSR内で先に現れる子。

`n=0` では辺なしの `build()` が成功し、`root() == -1`。辺を追加すると構築結果は無効になる。

## add / remove

```cpp
sack.run(add, remove, answer);
```

callbackのsignatureは次の通り。

```cpp
void add(int vertex);
void remove(int vertex);
void answer(int vertex);
```

callback側の管理状態は呼出し前に空でなければならない。`answer(v)` の呼出し時点では、`add` されていてまだ `remove` されていない頂点がちょうど $v$ の部分木になる。`answer` は各頂点について1回呼ばれる。`remove(v)` は以前の `add(v)` を取り消す操作でなければならない。頂点の追加順・削除順に依存しない集合情報を扱う。

全頂点の処理後に全ての追加が取り消され、callback側の状態は呼出し前と同じになる。

## add / reset

```cpp
sack.run_with_reset(add, reset, answer);
```

callbackのsignatureは次の通り。

```cpp
void add(int vertex);
void reset();
void answer(int vertex);
```

`answer(v)` の契約は `run` と同じ。処理済みlight childの状態を捨てるたびに `reset()` を呼び、最後にも1回呼ぶ。`reset` は現在追加済みの全頂点をまとめて消去する操作とする。変更した添字だけを記録して初期化する場合に使う。

## metadata

```cpp
sack.size();
sack.edge_count();
sack.root();
sack.parent(v);
sack.subtree_size(v);
sack.heavy_child(v);
sack.tin(v);
sack.tout(v);
sack.subtree_range(v);
sack.preorder_vertex(index);
```

`parent(root) == -1`、葉の `heavy_child(v) == -1`。頂点 $v$ の部分木はpreorder上の $[tin(v),tout(v))$ である。

## API別の時間計算量・空間計算量

$A,R,S,Q$ をそれぞれ `add`, `remove`, `reset`, `answer` 1回の時間計算量とする。

| API | 時間計算量 |
| --- | --- |
| `DsuOnTree(n)` | $O(1)$ |
| copy/move構築・代入 | $O(MAX_SIZE)$ |
| `size()` / `edge_count()` | $O(1)$ |
| `add_edge(u,v)` | $O(1)$ |
| `build(root)` | $O(n)$ |
| `root()` / `parent(v)` / `subtree_size(v)` / `heavy_child(v)` | $O(1)$ |
| `tin(v)` / `tout(v)` / `subtree_range(v)` / `preorder_vertex(i)` | $O(1)$ |
| `run(add,remove,answer)` | $O(n\log(n+1)(A+R+1)+nQ)$ |
| `run_with_reset(add,reset,answer)` | $O(n\log(n+1)(A+1)+n(S+Q))$ |

各頂点への `add` と `remove` はそれぞれ $O(\log(n+1))$ 回以下、`answer` は $n$ 回。`reset` はlight edge数+1回で高々 $n$ 回。

object本体は $O(MAX_SIZE)$。各APIの追加領域はcallback側の状態を除いて $O(1)$。

## 注意点

constructorは `0 <= n <= MAX_SIZE`、辺容量は `MAX_SIZE-1`。metadataと実行APIは直近の `add_edge` 後に成功した `build` を必要とする。

空木を除き、入力は頂点番号が $[0,n)$ の無向連結木とする。端点・root・indexの範囲違反、辺容量超過、辺数違反、自己loop・多重辺・cycle・非連結入力では `runtime_error` を送出する。

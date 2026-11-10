---
title: DSU on Tree (Sack) (木上の DSU)
documentation_of: ../../../../src/algorithm/tree/decomposition/dsu_on_tree.hpp
---

各頂点について、部分木全体の情報を求める。
部分木サイズが最大の子を heavy child として情報を保持し、light child の部分木だけを Euler 順に追加し直す。

## 型

```cpp
DsuOnTree<> dynamic(n);          // 可変長版
DsuOnTree<100000> fixed(n);      // 固定容量版
```

テンプレート引数 `MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。
明示すると、辺・CSR 形式の隣接列・Euler 順・部分木サイズ・heavy child・処理用のスタックを固定長配列で保持し、`build` と `run` は動的確保を行わない。
いずれの版も再帰は使わない。
実際の頂点数 $n$ は $0 \le n$ で、固定容量版では $n \le \texttt{MAX_SIZE}$ でもなければならない。

## 辺の追加と構築

```cpp
sack.add_edge(u, v);
sack.build(root);
```

無向辺をちょうど $n-1$ 本追加する。
`build` は根付き木の preorder と各部分木の半開区間を構築し、部分木サイズが最大の子を heavy child にする。
同率の場合は、CSR 内で先に現れる子を heavy child にする。

$n=0$ では辺なしの `build()` が成功し、`root() == -1` となる。辺を追加すると構築結果は無効になる。

## add / remove

```cpp
sack.run(add, remove, answer);
```

コールバックのシグネチャは次のとおりである。

```cpp
void add(int vertex);
void remove(int vertex);
void answer(int vertex);
```

コールバック側で管理する状態は、呼び出し前に空でなければならない。
`answer(v)` の呼び出し時点では、`add` されてまだ `remove` されていない頂点の集合が、ちょうど $v$ の部分木になる。
`answer` は各頂点について 1 回呼ばれる。`remove(v)` は以前の `add(v)` を取り消す操作でなければならない。
頂点の追加順・削除順に依存しない集合の情報を扱う。

全頂点の処理後にはすべての追加が取り消され、コールバック側の状態は呼び出し前と同じになる。

## add / reset

```cpp
sack.run_with_reset(add, reset, answer);
```

コールバックのシグネチャは次のとおりである。

```cpp
void add(int vertex);
void reset();
void answer(int vertex);
```

`answer(v)` の契約は `run` と同じである。
処理済みの light child の状態を捨てるたびに `reset()` を呼び、最後にも 1 回呼ぶ。
`reset` は現在追加済みのすべての頂点をまとめて消去する操作とする。変更した添字だけを記録して初期化する場合に使う。

## メタデータ

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

`parent(root) == -1`、葉では `heavy_child(v) == -1` である。
頂点 $v$ の部分木は preorder 上の $[\mathrm{tin}(v), \mathrm{tout}(v))$ である。

## API 別の時間計算量・空間計算量

$A, R, S, Q$ をそれぞれ `add`, `remove`, `reset`, `answer` の 1 回あたりの時間計算量とする。

| API | 時間計算量 |
| --- | --- |
| `DsuOnTree(n)` | 可変長版 $O(n)$、固定容量版 $O(1)$ |
| コピー・ムーブの構築と代入 | 可変長版 $O(n)$、固定容量版 $O(\texttt{MAX_SIZE})$ |
| `size()` / `edge_count()` | $O(1)$ |
| `add_edge(u, v)` | $O(1)$ |
| `build(root)` | $O(n)$ |
| `root()` / `parent(v)` / `subtree_size(v)` / `heavy_child(v)` | $O(1)$ |
| `tin(v)` / `tout(v)` / `subtree_range(v)` / `preorder_vertex(i)` | $O(1)$ |
| `run(add, remove, answer)` | $O(n\log(n+1)(A+R+1)+nQ)$ |
| `run_with_reset(add, reset, answer)` | $O(n\log(n+1)(A+1)+n(S+Q))$ |

各頂点への `add` と `remove` はそれぞれ $O(\log(n+1))$ 回以下、`answer` は全体で $n$ 回呼ばれる。
`reset` の呼び出し回数は light edge の本数 $+1$ で、高々 $n$ 回である。

オブジェクト本体の領域は可変長版で $O(n)$、固定容量版で $O(\texttt{MAX_SIZE})$ である。
各 API の追加領域は、コールバック側の状態を除いて $O(1)$ である。

## 注意点

- コンストラクタは `0 <= n`（固定容量版では `n <= MAX_SIZE` も）を要求する。`add_edge` は辺の容量（可変長版では $n-1$ 本、固定容量版では $\texttt{MAX_SIZE}-1$ 本）を検査する。
- メタデータと実行 API は、直近の `add_edge` の後に成功した `build` を必要とする。
- 空木を除き、入力は頂点番号が $[0, n)$ の無向連結木とする。
- 端点・根・添字の範囲外、辺の容量超過、辺数の違反、自己ループ・多重辺・閉路・非連結な入力では `std::runtime_error` を送出する。表のサイズが `int` の範囲を超える場合は `std::length_error` を送出する。

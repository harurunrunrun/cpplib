---
title: Offline Tree Edge Deletion Disconnected Pairs (木辺削除後の非連結頂点対数) [NITTROAD]
documentation_of: ../../../../src/structure/dsu/specialized/offline_tree_edge_deletion_disconnected_pairs.hpp
---

木から各辺を高々 1 回削除する操作列を逆順に処理し、各時点で非連結な頂点の非順序対の個数を求める。
辺を復元するときに DSU の成分サイズを使う。

## テンプレート引数

```cpp
OfflineTreeEdgeDeletionDisconnectedPairs<MAX_SIZE = -1>
```

`MAX_SIZE` は頂点数の上限。省略（`-1`）すると、`solve` 内部の DSU が実行時の頂点数に合わせて `std::vector` を確保する可変長版になる。

```cpp
using Dynamic = OfflineTreeEdgeDeletionDisconnectedPairs<>;
```

## コンストラクタ

```cpp
explicit OfflineTreeEdgeDeletionDisconnectedPairs(int vertex_count);
```

孤立した `vertex_count` 頂点と、空の操作列を作る。

## add_edge

```cpp
int add_edge(int left, int right)
```

木を構成する辺を登録し、その辺番号（$0$-indexed）を返す。
最終的な辺数は `max(0, vertex_count - 1)` でなければならない。

## delete_edge

```cpp
void delete_edge(int edge)
```

指定した番号の辺の削除を操作列に追加する。同じ辺の削除を 2 回追加することはできない。

## add_query

```cpp
void add_query()
```

その時点で非連結な頂点の非順序対の個数を求めるクエリを操作列に追加する。

## solve

```cpp
vector<long long> solve() const
```

各 `add_query()` に対する答えを追加順に返す。登録された辺が木であることもここで検査する。

## 時間計算量

$N$ を頂点数 `vertex_count`、操作数を $Q$、答えの個数を $A$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `add_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `delete_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `add_query` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve` | $O(\texttt{MAX_SIZE}+(N+Q)\alpha(N))$ | $O(\texttt{MAX_SIZE}+N+Q+A)$ |

可変長版では、上の表の $\texttt{MAX_SIZE}$ の項は $N$ に置き換わる。

## 空間計算量

本体は $O(N+Q)$。`solve` は内部で DSU を 2 つ作るため、固定長版では一時領域として $O(\texttt{MAX_SIZE})$ を使う。

## 注意点

- $0 \le N \le \texttt{MAX_SIZE}$ でなければならない。違反するとコンストラクタが `std::runtime_error` を送出する。
- 登録する辺は、頂点集合 $[0, N)$ 上の木でなければならない。
- 辺の本数・頂点番号・削除する辺の番号の範囲違反と重複削除では `std::runtime_error` を、閉路を含む辺の登録では（`solve` の時点で）`std::invalid_argument` を送出する。
- `solve` は状態を変更せず、同じ操作列に対して繰り返し呼べる。

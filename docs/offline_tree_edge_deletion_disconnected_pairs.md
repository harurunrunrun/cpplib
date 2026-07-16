---
title: Offline Tree Edge Deletion Disconnected Pairs (木辺削除後の非連結頂点対数) [NITTROAD]
documentation_of: ../src/structure/dsu/offline_tree_edge_deletion_disconnected_pairs.hpp
---

木から各辺を高々1回削除する操作列を逆順に処理し、各時点の非連結なunordered頂点対数を求める。
辺の復元時にDSUの成分サイズを使う。

## `OfflineTreeEdgeDeletionDisconnectedPairs<MAX_SIZE>`

### コンストラクタ

```cpp
explicit OfflineTreeEdgeDeletionDisconnectedPairs(int vertex_count);
```

孤立した `vertex_count` 頂点と空の操作列を作る。

### `add_edge(left, right)`

木を構成する辺を登録し、その0-indexed辺番号を返す。
最終的に辺数は `max(0, vertex_count - 1)` でなければならない。

### `delete_edge(edge)`

指定した辺番号の削除を操作列へ追加する。
同じ辺を2回追加できない。

### `add_query()`

現在非連結であるunordered頂点対数を求めるqueryを操作列へ追加する。

### `solve()`

各 `add_query()` に対する回答を追加順に返す。
登録辺が木であることも検査する。

## API別の時間計算量・空間計算量

$N=vertex\_count$、操作数を $Q$、回答数を $A$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `add_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `delete_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `add_query` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve` | $O(MAX\_SIZE+(N+Q)\alpha(N))$ | $O(MAX\_SIZE+N+Q+A)$ |

## 注意点

- $0\leq N\leq MAX\_SIZE$ でなければならない。
- 登録辺は頂点 $[0,N)$ 上の木でなければならない。
- 本数、頂点番号、削除番号、重複削除の違反時は `runtime_error`、閉路を含む登録辺には `invalid_argument` を送出する。
- `solve` は状態を変更せず、同じ操作列に対して繰り返し呼べる。

---
title: Parallel Binary Search (並列二分探索)
documentation_of: ../../../../../src/algorithm/other/offline_query/batch_framework/parallel_binary_search.hpp
---

操作を先頭から順に適用した状態に対する複数の単調な判定を、操作列の走査を共有しながら同時に二分探索する。

## `parallel_binary_search`

```cpp
template<class Reset, class Apply, class Predicate>
vector<int> parallel_binary_search(
    int query_count,
    int operation_count,
    Reset&& reset,
    Apply&& apply,
    Predicate&& predicate
)
```

各クエリ `q` について、先頭から `t` 個の操作を適用した状態で `predicate(q)` が初めて真になる `t` を返す。答えは $0$ 以上 `operation_count` 以下であり、最後まで偽なら番兵値 `operation_count + 1` を返す。したがって、操作を 1-indexed で数える問題では、正の返り値をそのまま最初の操作番号として使える。

callback の意味は次のとおり。

- `reset()`: 操作を 0 個適用した初期状態へ戻す。
- `apply(i)`: 0-indexed の操作 `i` を現在の状態へ適用する。
- `predicate(q)`: 現在の状態で 0-indexed のクエリ `q` が成立するかを返す。

各ラウンドでは `reset()` を 1 回呼び、必要な最大の接頭辞まで `apply(0), apply(1), ...` の順に呼ぶ。適用数 `t` のバケットに属するクエリの `predicate` を呼ぶ直前には、ちょうど `t` 個の操作が適用されている。同じバケットの `predicate` はクエリ番号の昇順に呼ぶ。未確定のクエリが残っていなければ次のラウンドは開始しない。`query_count == 0` では callback を一度も呼ばない。

## 計算量

$Q$ を `query_count`、$M$ を `operation_count`、$L=\lceil\log_2(M+2)\rceil$ とする。`reset` 1 回、`apply` 1 回、`predicate` 1 回の時間をそれぞれ $R, A, P$ とすると、最悪計算量は次のとおり。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `parallel_binary_search` | $O(L(R+M(A+1)+Q(P+1)))$ | $O(M+Q)$ |

callback が保持するデータ構造の領域は追加領域に含めない。

## 注意点

- 各クエリについて、操作の接頭辞を伸ばしたときの `predicate` は偽から真へ高々 1 回だけ変化しなければならない。真から偽へ戻る場合、返り値は保証されない。
- `reset` は毎ラウンド同じ初期状態を復元し、`apply(i)` は操作 `0, ..., i - 1` が順に適用済みの状態を、操作 `0, ..., i` が適用済みの状態に遷移させなければならない。
- `predicate` による状態変更が、その後の `predicate` や `apply` の結果へ影響してはならない。
- `query_count` と `operation_count` は非負でなければならない。また返り値の番兵値を `int` で表すため、`operation_count != INT_MAX` が必要である。違反時は callback を呼ばずに `invalid_argument` を送出する。
- callback が送出した例外はそのまま外へ伝播する。その場合、callback が管理する外部状態は途中のラウンドの状態になり得る。
- `operation_count == 0` でも初期状態で `predicate` を評価し、最初から真なら $0$、偽なら $1$ を返す。

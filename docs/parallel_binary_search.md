---
title: Parallel Binary Search (並列二分探索) [METEORS]
documentation_of: ../src/algorithm/other/parallel_binary_search.hpp
---

操作を先頭から適用した状態に対する複数の単調判定を、操作列の走査を共有して同時に二分探索する。

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

query `q` ごとに、先頭から `t` 個の操作を適用した状態で `predicate(q)` が初めて真になる `t` を返す。答えは `0` 以上 `operation_count` 以下であり、最後まで偽なら sentinel `operation_count + 1` を返す。したがって、操作を1-indexedで数える問題では、正の返り値をそのまま最初の操作番号として使える。

callback の意味は次のとおり。

- `reset()` は、操作を0個適用した初期状態へ戻す。
- `apply(i)` は、0-indexedの操作 `i` を現在の状態へ適用する。
- `predicate(q)` は、現在の状態で0-indexedのquery `q` が成立するかを返す。

各roundでは `reset()` を1回呼び、必要な最大prefixまで `apply(0), apply(1), ...` の順に呼ぶ。適用数 `t` のbucketに属するqueryについて、その直前までにちょうど `t` 個の操作が適用されている。同じbucketの `predicate` はquery番号の昇順に呼ぶ。終了済みqueryしかない場合は次のroundを開始しない。`query_count == 0` ではcallbackを一度も呼ばない。

`Q = query_count`、`M = operation_count`、$L=\lceil\log_2(M+2)\rceil$ とする。`reset`、1回の `apply`、1回の `predicate` の時間をそれぞれ $R,A,P$ とすると、worst-caseの計算量は次のとおり。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `parallel_binary_search` | $O(L(R+M(A+1)+Q(P+1)))$ | $O(M+Q)$ |

callbackが保持するデータ構造の領域は追加領域に含めない。

## 注意点

- 各queryについて、操作prefixを伸ばしたときの `predicate` は偽から真へ高々1回だけ変化しなければならない。真から偽へ戻る場合、返り値は保証されない。
- `reset` は毎round同じ初期状態を復元し、`apply(i)` は操作 `0, ..., i - 1` が順に適用済みの状態を操作 `0, ..., i` 適用済みに遷移させなければならない。
- `predicate` による状態変更は、その後の `predicate` や `apply` の結果へ影響してはならない。
- `query_count` と `operation_count` は非負でなければならない。また返り値のsentinelを `int` で表すため、`operation_count != INT_MAX` が必要。違反時はcallbackを呼ばず `invalid_argument` を送出する。
- callbackが送出した例外はそのまま外へ伝播する。その場合、callbackが管理する外部状態は途中のroundの状態になり得る。
- `operation_count == 0` でも初期状態の `predicate` を評価し、最初から真なら `0`、偽なら `1` を返す。

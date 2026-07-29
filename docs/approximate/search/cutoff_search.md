---
title: Cutoff and Bounded Search (打ち切り・制限付き探索)
documentation_of: ../../../src/approximate/search/cutoff_search.hpp
---

非負辺の状態グラフに対する制限付き A* 群と、最小化問題用の汎用
Branch and Bound を提供する。各 A* API の `neighbors(state)` は
`pair<State, Cost>` の range、`goal(state)` は真偽値、
`heuristic(state)` は有限で非負な推定残りコストを返す。

This header provides resource-bounded A* variants for non-negative state
graphs and generic best-first branch-and-bound for minimization.
各 API の時間計算量は以下で個別に示す。

## 結果型 / Result types

```cpp
enum class CutoffSearchStop;
template<class State, class Cost> struct CutoffSearchResult;
template<class State, class Value> struct BranchAndBoundSearchResult;
```

`CutoffSearchResult` は `cost`, `path`, `expanded`, `generated`,
`peak_frontier`, `stop` を持つ。`BranchAndBoundSearchResult` は
`solution`, `objective`, `expanded`, `generated`, `optimal`, `stop` を持つ。
`operator bool()` は解の有無を返し、すべてのメンバ参照は $O(1)$。

`CutoffSearchStop` distinguishes exhaustion, goal, depth, score, expansion,
and memory cutoffs. Result field access and conversion to `bool` are $O(1)$.

## `bounded_suboptimal_search`

```cpp
bounded_suboptimal_search(
    start, goal, neighbors, heuristic, factor, hash = {}, equal = {}
)
```

OPEN の最小 $f=g+h$ の `factor` 倍以内を focal set とし、その中で
$h$ 最小の状態を展開する。許容的ヒューリスティックなら返す解は最適値の
`factor` 倍以下。`factor` は有限で 1 以上。

It is focal search: expand the lowest-$h$ node whose $f$ is within `factor`
of the anchor. With an admissible heuristic it is factor-suboptimal.
発見状態数を $V$、生成辺数を $E$ とすると、vector 上の OPEN 走査により
時間 $O(V^2+E)$、空間 $O(V)$。

## `depth_bounded_a_star_search`

```cpp
depth_bounded_a_star_search(
    start, goal, neighbors, heuristic, maximum_depth, hash = {}, equal = {}
)
```

深さが `maximum_depth` の状態を展開せずに A* を行う。境界で子を切った場合は
`depth_limit`。This is A* with an inclusive node-depth bound.
時間 $O(V^2+E)$、空間 $O(V)$。

## `cutoff_a_star_search`

```cpp
cutoff_a_star_search(
    start, goal, neighbors, heuristic, maximum_f, hash = {}, equal = {}
)
```

$g+h>maximum_f$ の状態を生成しない A*。打ち切りが起きれば `cost_limit`。
It prunes nodes above an inclusive $f$ cutoff.
時間 $O(V^2+E)$、空間 $O(V)$。

## `expansion_limited_a_star_search`

```cpp
expansion_limited_a_star_search(
    start, goal, neighbors, heuristic, maximum_expansions,
    hash = {}, equal = {}
)
```

展開直前に上限を検査するため、上限 0 では始点も検査しない。
It stops before expansion number `maximum_expansions + 1`.
時間 $O(V^2+E)$、空間 $O(V)$、実際の展開数は上限以下。

## `memory_limited_a_star_search`

```cpp
memory_limited_a_star_search(
    start, goal, neighbors, heuristic, maximum_frontier,
    hash = {}, equal = {}
)
```

各展開後、OPEN が上限を超えた間は $f$ 最大（同値なら新しい）状態を捨てる。
This bounded-frontier A* discards the worst open nodes and may therefore miss
an existing solution. 時間 $O(V^2+EV)$、保存ノード $O(V)$、同時 OPEN は
$O(maximum\_frontier)$。`maximum_frontier == 0` は不正。

## `epsilon_greedy_search`

```cpp
epsilon_greedy_search(
    start, goal, neighbors, heuristic, epsilon, maximum_expansions,
    seed = 0, hash = {}, equal = {}
)
```

確率 `epsilon` で OPEN から一様ランダム、それ以外は $f$ 最小を選ぶ。
The seed makes randomized choices reproducible. 時間 $O(V^2+E)$、空間
$O(V)$。`epsilon` は有限な $[0,1]$。

## `best_first_branch_and_bound`

```cpp
best_first_branch_and_bound(
    root, complete, branch, objective, lower_bound
)
```

`lower_bound(state)` 最小の部分問題から展開し、完全解の incumbent 以上の
下界を枝刈りする最小化 Branch and Bound。`branch(state)` は子状態の range。
It exhausts every node that can beat the incumbent, so `optimal` is true on
return when the supplied bounds are valid.

生成状態を $N$、生成辺を $E$ とすると時間 $O((N+E)\log N)$、空間 $O(N)$。
各 callback 呼び出しは、記載した回数に callback 自身の計算量を掛ける。

## `interrupted_branch_and_bound`

```cpp
interrupted_branch_and_bound(
    root, complete, branch, objective, lower_bound, maximum_expansions
)
```

同じ best-first 規則を用いるが、展開数上限で incumbent を保持して停止する。
It returns the incumbent on interruption and sets `optimal == false`.
上限を $L$ とすると時間 $O((L+E_L)\log N_L)$、空間 $O(N_L)$。

## 注意点

Notes:

- 辺コスト、加算結果、callback の値は各型の表現範囲内でなければならない。
  Costs and callback values must remain representable.
- A* の最適性・近似率には許容的ヒューリスティックが必要。メモリ制限版と
  ε-greedy 版は一般に完全でも最適でもない。
- Branch and Bound は最小化専用。`lower_bound` が真の下界でない場合、
  `optimal` の数学的意味は失われる。
- ハッシュ表の状態同一性は `Hash` と `Equal` が一貫して定義する。

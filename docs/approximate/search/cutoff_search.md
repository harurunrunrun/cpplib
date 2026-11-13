---
title: Cutoff and Bounded Search (打ち切り・制限付き探索)
documentation_of: ../../../src/approximate/search/cutoff_search.hpp
---

非負の辺を持つ状態グラフに対する、資源制限付きの A* 群と、最小化問題用の汎用の最良優先分枝限定法（Branch and Bound）を提供する。各 A* API の `neighbors(state)` は `pair<State, Cost>` の範囲、`goal(state)` は真偽値、`heuristic(state)` は有限かつ非負の推定残りコストを返す。

以下、発見状態数を $V$、生成辺数を $E$ とする。

## 結果型

```cpp
enum class CutoffSearchStop;
template<class State, class Cost> struct CutoffSearchResult;
template<class State, class Value> struct BranchAndBoundSearchResult;
```

`CutoffSearchResult` は `cost`, `path`, `expanded`, `generated`, `peak_frontier`, `stop` を持つ。`BranchAndBoundSearchResult` は `solution`, `objective`, `expanded`, `generated`, `optimal`, `stop` を持つ。`operator bool()` は解の有無を返し、すべてのメンバの参照は $O(1)$。

`CutoffSearchStop` は停止理由として、探索空間の枯渇（`exhausted`）、ゴール発見（`goal_found`）、深さ上限（`depth_limit`）、コスト上限（`cost_limit`）、展開数上限（`expansion_limit`）、メモリ上限（`memory_limit`）を区別する。

## `bounded_suboptimal_search`

```cpp
bounded_suboptimal_search(
    start, goal, neighbors, heuristic, factor, hash = {}, equal = {}
)
```

OPEN の最小の $f=g+h$ の `factor` 倍以内を FOCAL 集合とし、その中で $h$ が最小の状態を展開する（focal search）。許容的なヒューリスティックなら、返す解のコストは最適値の `factor` 倍以下である。`factor` は 1 以上の有限値でなければならない。

`std::vector` 上の OPEN を線形に走査するため、時間計算量は $O(V^2+E)$、空間計算量は $O(V)$。

## `depth_bounded_a_star_search`

```cpp
depth_bounded_a_star_search(
    start, goal, neighbors, heuristic, maximum_depth, hash = {}, equal = {}
)
```

深さが `maximum_depth` の状態は展開しない A* である（上限の深さの節点自体は含む）。境界で子を切った場合、停止理由は `depth_limit` になる。
時間計算量は $O(V^2+E)$、空間計算量は $O(V)$。

## `cutoff_a_star_search`

```cpp
cutoff_a_star_search(
    start, goal, neighbors, heuristic, maximum_f, hash = {}, equal = {}
)
```

$g+h$ が `maximum_f` を超える状態を生成しない A* である（`maximum_f` に等しい状態は生成する）。打ち切りが起きた場合、停止理由は `cost_limit` になる。
時間計算量は $O(V^2+E)$、空間計算量は $O(V)$。

## `expansion_limited_a_star_search`

```cpp
expansion_limited_a_star_search(
    start, goal, neighbors, heuristic, maximum_expansions,
    hash = {}, equal = {}
)
```

展開の直前に上限を検査し、`maximum_expansions + 1` 回目の展開の前で停止する。上限 0 では始点も検査しない。
時間計算量は $O(V^2+E)$、空間計算量は $O(V)$ で、実際の展開数は上限以下である。

## `memory_limited_a_star_search`

```cpp
memory_limited_a_star_search(
    start, goal, neighbors, heuristic, maximum_frontier,
    hash = {}, equal = {}
)
```

各展開の後、OPEN が上限を超えている間は $f$ が最大（同値なら新しい方）の状態を捨てる。そのため、解が存在しても見つからないことがある。
時間計算量は $O(V^2+EV)$、保存する節点数は $O(V)$、同時に OPEN にある状態は高々 `maximum_frontier` 個である。

## `epsilon_greedy_search`

```cpp
epsilon_greedy_search(
    start, goal, neighbors, heuristic, epsilon, maximum_expansions,
    seed = 0, hash = {}, equal = {}
)
```

確率 `epsilon` で OPEN から一様ランダムに、それ以外は $f$ が最小の状態を選んで展開する。乱択はシード `seed` で再現できる。
時間計算量は $O(V^2+E)$、空間計算量は $O(V)$。`epsilon` は $[0,1]$ の有限値でなければならない。

## `best_first_branch_and_bound`

```cpp
best_first_branch_and_bound(
    root, complete, branch, objective, lower_bound
)
```

`lower_bound(state)` が最小の部分問題から展開し、下界が暫定解（incumbent）の値以上の部分問題を枝刈りする、最小化用の分枝限定法である。`branch(state)` は子状態の範囲を返す。暫定解を改善し得る節点をすべて調べ尽くすため、与えた下界が正しければ、返った時点で `optimal` は真である。

生成状態数を $N$、生成辺数を $E$ とすると、時間計算量は $O((N+E)\log N)$、空間計算量は $O(N)$。各コールバックの計算量は、これに呼び出し回数を掛けて加わる。

## `interrupted_branch_and_bound`

```cpp
interrupted_branch_and_bound(
    root, complete, branch, objective, lower_bound, maximum_expansions
)
```

同じ最良優先の規則を使うが、展開数の上限に達すると暫定解を保持したまま停止する。中断した場合は暫定解を返し、`optimal == false` とする。
上限までに生成した状態数・辺数を $N_L$, $E_L$、上限を $L$ とすると、時間計算量は $O((L+E_L)\log N_L)$、空間計算量は $O(N_L)$。

## 注意点

- 負の辺、有限でないか負のヒューリスティック値、1 未満または有限でない `factor`、有限でない `maximum_f`、$[0,1]$ の外の `epsilon`、`maximum_frontier == 0` に対しては `std::invalid_argument` を送出する。
- 辺のコスト、加算結果、コールバックの値は各型の表現範囲に収まらなければならない。
- A* の最適性や近似比の保証には、許容的なヒューリスティックが必要である。メモリ制限版と ε-greedy 版は、一般には完全でも最適でもない。
- 分枝限定法は最小化専用である。`lower_bound` が真の下界でない場合、`optimal` の数学的な意味は失われる。
- ハッシュ表での状態の同一性は、`Hash` と `Equal` が一貫して定義する。

---
title: Heuristic Search
documentation_of: ../src/approximate/search/heuristic_search.hpp
---

状態をキーとする汎用ヒューリスティック探索。状態型を整数に限定せず、近傍生成・ゴール判定・ヒューリスティックを callable として渡す。

# 共通の型と callable

`neighbors(state)` は `pair<State, Cost>` の range を返す。`second` は非負の辺コストでなければならない。`goal(state)` は真偽値、`heuristic(state)` は残りコストの推定値を返す。

```cpp
template<class State, class Cost>
struct HeuristicSearchResult {
    optional<Cost> cost;
    vector<State> path;
    size_t expanded;
};
```

- `cost`: 見つかった経路のコスト。未発見なら `nullopt`。
- `path`: 始点と終点を含む復元経路。未発見なら空。
- `expanded`: キューから有効な状態を取り出して展開した回数。再展開も数える。

`Hash` と `Equal` を省略すると `std::hash<State>` と `std::equal_to<State>` を用いる。ハッシュ表の操作は期待 $O(1)$ と仮定する。

# `a_star_search`

```cpp
a_star_search(start, goal, neighbors, heuristic, hash = {}, equal = {})
```

$f(v)=g(v)+h(v)$ の小さい状態から展開する。ヒューリスティックが許容的なら最適経路を返す。不整合なヒューリスティックも再オープンによって扱えるが、再展開が増える。

生成辺数を $A$、有効な展開回数を $X$、同時に保持するキュー要素数の最大を $Q$ とすると、時間計算量は $O((A+X)\log Q)$、空間計算量は発見状態数を $V_d$ として $O(V_d+Q)$。

# `weighted_a_star_search`

```cpp
weighted_a_star_search(start, goal, neighbors, heuristic, weight,
                       hash = {}, equal = {})
```

$f(v)=g(v)+w h(v)$ を使う。`weight >= 0` が必要。$w>1$ では一般に最適性を失う代わりに探索を減らせる。$w=1$ は A* と同じ優先度になる。

時間計算量は $O((A+X)\log Q)$、空間計算量は $O(V_d+Q)$。

# `greedy_best_first_search`

```cpp
greedy_best_first_search(start, goal, neighbors, heuristic,
                         hash = {}, equal = {})
```

$h(v)$ だけで優先順位を決める。各状態は最初に発見した経路だけを保持するため、完全な有限グラフでは到達経路を返すが、最短性は保証しない。

生成辺数を $A$、発見状態数を $V_d$ とすると、時間計算量は $O((A+V_d)\log V_d)$、空間計算量は $O(V_d)$。

# `bidirectional_a_star_search`

```cpp
bidirectional_a_star_search(start, goal,
    forward_neighbors, backward_neighbors,
    forward_heuristic, backward_heuristic,
    hash = {}, equal = {})
```

始点側と終点側から A* を行い、両側で発見した状態を接続する。`backward_neighbors(v)` は元の有向辺を逆向きに辿る近傍を返す。無向グラフでは両方向に同じ近傍生成器を渡せる。各ヒューリスティックは、それぞれ反対側の固定端点までの許容的な下界でなければ最短性を保証しない。

両探索の生成辺数合計を $A$、展開回数を $X$、発見状態数合計を $V_d$ とすると、時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d)$。

# 例外・数値条件

- 負辺を生成すると `std::invalid_argument`。
- Weighted A* の負の重みは `std::invalid_argument`。
- `Cost` の加算、優先度の加算・乗算は表現可能範囲に収まる必要がある。
- `neighbors` が返す range は、その呼び出しの走査終了まで有効でなければならない。

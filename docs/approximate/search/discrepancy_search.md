---
title: Discrepancy Tree Search (相違数制限木探索)
documentation_of: ../../../src/approximate/search/discrepancy_search.hpp
---

`children(state)` が良い順に並べた子状態の range を返す探索木用 API。
現在経路上の重複は `Hash` と `Equal` で除く。
These APIs search an ordered tree and interpret the first child as the
heuristic-preferred choice.
各 API の時間計算量は以下で個別に示す。

## 結果型 / Result type

```cpp
enum class DiscrepancySearchStop;
template<class State> struct DiscrepancySearchResult;
```

結果は `solution`, `path`, `expanded`, `generated`, `iterations`, `stop` を持つ。
`operator bool()` と各メンバ参照は $O(1)$。停止理由は goal、深さ、相違数、
展開数、幅、完全枯渇を区別する。

## `limited_discrepancy_search`

```cpp
limited_discrepancy_search(
    root, goal, children, maximum_depth, maximum_discrepancies,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

非先頭の子を選ぶ回数を discrepancy とし、上限を 0 から反復的に増やす LDS。
Classic LDS repeats depth-first search with an increasing discrepancy budget.
最大分岐数 $b$、深さ $D$、上限 $K$ の最悪時間は
$O(\sum_{k=0}^{K}\sum_{i=0}^{k}{D\choose i}(b-1)^i)$、空間 $O(D)$。

## `depth_bounded_discrepancy_search`

```cpp
depth_bounded_discrepancy_search(
    root, goal, children, maximum_depth,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

根から discrepancy を許す深さを 0 から $D$ まで増やす DDS。各反復では
その境界以深で先頭の子だけを選ぶ。
DDS progressively moves the depth boundary below which non-preferred choices
are forbidden. 最悪時間 $O(D b^D)$、空間 $O(D)$。

## `improved_limited_discrepancy_search`

```cpp
improved_limited_discrepancy_search(
    root, goal, children, maximum_depth, maximum_discrepancies,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

反復 $k$ で discrepancy がちょうど $k$ の未完了経路だけを展開する ILDS。
短い位置の goal はその時点で受理する。ILDS avoids revisiting paths assigned
to earlier discrepancy layers. 最悪時間
$O(\sum_{i=0}^{K}{D\choose i}(b-1)^i)$、空間 $O(D)$。

## `iterative_broadening_search`

```cpp
iterative_broadening_search(
    root, goal, children, maximum_depth, maximum_width,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

各節点で先頭から許す子数を 1 から反復的に増やす Iterative Broadening。
It repeats depth-first search with increasing per-node branching width.
幅上限 $W$ の最悪時間は $O(\sum_{w=1}^{W}w^D)=O(W^{D+1})$、
空間 $O(D)$。`maximum_width == 0` は不正。

## 注意点

Notes:

- `maximum_depth` は根を深さ 0 とする inclusive な節点深さ上限。
- `maximum_expansions == 0` では根の goal 判定も行わない。
- 順序がヒューリスティックの良さを表さなければ、discrepancy の意味は薄れる。
  These are incomplete whenever depth, discrepancy, width, or expansion
  resources are insufficient.
- 再帰深さは $O(D)$。非常に大きい `maximum_depth` ではスタック容量に注意する。

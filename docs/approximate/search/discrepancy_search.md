---
title: Discrepancy Tree Search (相違数制限木探索)
documentation_of: ../../../src/approximate/search/discrepancy_search.hpp
---

`children(state)` が良い順に並べた子状態の範囲を返す探索木に対する API である。先頭の子をヒューリスティックが推す選択とみなし、現在の経路上の重複は `Hash` と `Equal` で除く。

以下、最大分岐数を $b$、深さの上限 `maximum_depth` を $D$ とする。

## 結果型

```cpp
enum class DiscrepancySearchStop;
template<class State> struct DiscrepancySearchResult;
```

結果は `solution`, `path`, `expanded`, `generated`, `iterations`, `stop` を持つ。`operator bool()` と各メンバの参照は $O(1)$。停止理由 `stop` は、ゴール発見（`goal_found`）、深さ上限（`depth_limit`）、相違数上限（`discrepancy_limit`）、展開数上限（`expansion_limit`）、幅上限（`broadening_limit`）、探索空間の枯渇（`exhausted`）を区別する。

## `limited_discrepancy_search`

```cpp
limited_discrepancy_search(
    root, goal, children, maximum_depth, maximum_discrepancies,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

先頭以外の子を選ぶ回数を相違数（discrepancy）とし、その上限を 0 から 1 ずつ増やしながら深さ優先探索を繰り返す LDS である。
相違数の上限 `maximum_discrepancies` を $K$ とすると、最悪時間計算量は $O(\sum_{k=0}^{K}\sum_{i=0}^{k}\binom{D}{i}(b-1)^i)$、空間計算量は $O(D)$。

## `depth_bounded_discrepancy_search`

```cpp
depth_bounded_discrepancy_search(
    root, goal, children, maximum_depth,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

根から相違を許す深さの境界を 0 から $D$ まで増やす DDS である。各反復では、その境界より深い位置では先頭の子だけを選ぶ。
最悪時間計算量は $O(D b^D)$、空間計算量は $O(D)$。

## `improved_limited_discrepancy_search`

```cpp
improved_limited_discrepancy_search(
    root, goal, children, maximum_depth, maximum_discrepancies,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

反復 $k$ では相違数がちょうど $k$ の未完了の経路だけを展開し、前の反復で調べた経路の再訪を避ける ILDS である。より浅い位置にあるゴールはその時点で受理する。
最悪時間計算量は $O(\sum_{i=0}^{K}\binom{D}{i}(b-1)^i)$、空間計算量は $O(D)$。

## `iterative_broadening_search`

```cpp
iterative_broadening_search(
    root, goal, children, maximum_depth, maximum_width,
    maximum_expansions = size_t max, hash = {}, equal = {}
)
```

各節点で先頭から何個の子まで許すか（幅）を 1 から増やしながら深さ優先探索を繰り返す Iterative Broadening である。
幅の上限を $W$ とすると、最悪時間計算量は $O(\sum_{w=1}^{W} w^D) = O(W^{D+1})$、空間計算量は $O(D)$。

## 注意点

- `maximum_width == 0` に対しては `std::invalid_argument` を送出する。
- `maximum_depth` は根を深さ 0 とする節点の深さの上限で、上限の深さ自体も含む。
- `maximum_expansions == 0` では根のゴール判定も行わない。
- 子の順序がヒューリスティックとしての良さを表していなければ、相違数の意味は薄れる。
- 深さ・相違数・幅・展開数のいずれかの上限が足りない場合、解が存在しても見つからないことがある（完全ではない）。
- 再帰の深さは $O(D)$ である。非常に大きい `maximum_depth` ではスタック容量に注意する。

---
title: Randomized Minimum Cut
documentation_of: ../src/approximate/randomized/randomized_min_cut.hpp
---

無向多重グラフの全域最小cutを辺縮約で探索する。辺の重複は多重辺として扱い、
self-loopはcutに寄与しない。頂点は`0..vertex_count-1`である。

## `UndirectedEdge`

`first`と`second`を端点に持つ無向辺を表す。範囲外の端点があれば
`std::out_of_range`を送出する。

## `RandomizedMinCutResult`

- `cut_size`: 得られたcutを横切る辺数。
- `side[v]`: 頂点`v`の側を0または1で表す。連結グラフでは両方の値が現れる。

`side[0]`は常に0である。非連結グラフでは異なる連結成分を分けたcut size 0を返す。

## `karger_min_cut`

```cpp
auto result = karger_min_cut(vertex_count, edges, trials, random);
```

Karger法を`trials`回独立に実行し、最小の結果を返す。1回の成功確率は
少なくとも $2/(n(n-1))$ なので、失敗確率は高々
$(1-2/(n(n-1)))^{trials}$ である。頂点数を$n$、辺数を$m$とすると、
この実装は各縮約前に有効辺を走査するため時間$O(trials\,nm)$、
追加領域$O(n+m)$。`trials==0`は、cutが自明でない連結グラフでは
`std::invalid_argument`となる。

## `karger_stein_min_cut`

```cpp
auto result = karger_stein_min_cut(vertex_count, edges, repetitions, random);
```

各再帰で連結成分数を $\lceil n/\sqrt2\rceil$ まで独立に2回縮約する
Karger--Stein法を`repetitions`回実行する。成分数6以下では残った成分のcutを
全列挙する。1回の成功確率は $\Omega(1/\log n)$、反復により失敗確率は指数的に
減少する。この実装では各縮約と6成分以下の全列挙で元の全辺を走査するため、
1 repetitionあたりの保守的な上界は時間$O(n^2m)$、再帰stackを含む追加領域
$O(n\log n+m)$である。API全体では時間に`repetitions`を掛ける。
`repetitions==0`の扱いは`karger_min_cut`と同じ。

乱数生成器は`std::uniform_int_distribution`で利用可能でなければならない。
辺数と頂点数は`std::size_t`に収まる必要がある。同じseedと同じ標準ライブラリ実装では
同じ結果になるが、確率的保証はseedを独立に選ぶ場合のものである。

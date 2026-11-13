---
title: Randomized Minimum Cut (乱択最小カット)
documentation_of: ../../../src/approximate/randomized/randomized_min_cut.hpp
---

無向多重グラフの全域最小カットを、辺の縮約によって探索する。重複した辺は多重辺として扱い、自己ループはカットに寄与しない。頂点番号は $0$ から `vertex_count - 1` である。

## `UndirectedEdge`

端点 `first`, `second` を持つ無向辺を表す。API に範囲外の端点を持つ辺を渡すと `std::out_of_range` を送出する。

## `RandomizedMinCutResult`

- `cut_size`: 得られたカットを横切る辺の本数。
- `side[v]`: 頂点 `v` が属する側を 0 または 1 で表す。頂点が 2 つ以上あれば両方の値が現れる。

`side[0]` は常に 0 である。非連結なグラフでは、連結成分を分けるサイズ 0 のカットを返す。

## `karger_min_cut`

```cpp
auto result = karger_min_cut(vertex_count, edges, trials, random);
```

Karger 法を `trials` 回独立に実行し、最小の結果を返す。頂点数を $n$、辺数を $m$、`trials` を $T$ とする。
1 回の成功確率は少なくとも $2/(n(n-1))$ なので、失敗確率は高々 $(1 - 2/(n(n-1)))^T$ である。
この実装は縮約のたびに有効な辺を走査するため、時間計算量は $O(Tnm)$、追加空間計算量は $O(n + m)$。

## `karger_stein_min_cut`

```cpp
auto result = karger_stein_min_cut(vertex_count, edges, repetitions, random);
```

各再帰で連結成分数が $\lceil n/\sqrt{2} \rceil$ になるまでの縮約を独立に 2 回行う Karger–Stein 法を、`repetitions` 回実行する。成分数が 6 以下になったら、残った成分の分け方を全列挙する。
1 回の成功確率は $\Omega(1/\log n)$ で、反復により失敗確率は指数的に減少する。
この実装では各縮約と 6 成分以下での全列挙で元の全辺を走査するため、1 回あたりの保守的な上界は時間計算量 $O(n^2 m)$、再帰スタックを含む追加空間計算量 $O(n \log n + m)$ である。全体の時間は `repetitions` 倍になる。

## 注意点

- `trials == 0`（`repetitions == 0`）は、頂点が 2 つ以上の連結グラフに対しては `std::invalid_argument` を送出する。頂点数 1 以下または非連結のグラフでは試行せずに結果を返す。
- 乱数生成器は `std::uniform_int_distribution` で使えるものでなければならない。辺数と頂点数は `std::size_t` に収まる必要がある。
- 同じシードと同じ標準ライブラリ実装では同じ結果になるが、確率的保証はシードを独立に選んだ場合のものである。

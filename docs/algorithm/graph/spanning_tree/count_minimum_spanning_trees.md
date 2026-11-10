---
title: Count Minimum Spanning Trees (最小全域木数え上げ)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/count_minimum_spanning_trees.hpp
---

重み付き無向多重グラフの最小全域木の重みと個数を求める。Kruskal 法で同じ重みの辺をまとめて処理して縮約し、縮約した各多重グラフに行列木定理（Matrix-Tree 定理）を適用する。

## `MinimumSpanningTreeCountEdge<Weight>`

```cpp
template <class Weight>
struct MinimumSpanningTreeCountEdge {
    int from;
    int to;
    Weight weight;
};
```

無向辺の両端と重みを表す。多重辺は異なる辺として数え、自己ループは全域木の候補から除外する。

## `MinimumSpanningTreeCountResult<Weight>`

```cpp
template <class Weight>
struct MinimumSpanningTreeCountResult {
    bool connected;
    Weight minimum_weight;
    std::uint64_t count_mod;
};
```

- `connected`: 全域木が存在するなら真
- `minimum_weight`: 最小全域木の重みの総和
- `count_mod`: 最小全域木の個数を `MOD` で割った余り

非連結なら `connected == false`、`minimum_weight == Weight{}`、`count_mod == 0` を返す。

## `count_minimum_spanning_trees`

```cpp
template <std::uint64_t MOD, class Weight>
MinimumSpanningTreeCountResult<Weight> count_minimum_spanning_trees(
    int vertex_count,
    const std::vector<MinimumSpanningTreeCountEdge<Weight>>& edges
);
```

`vertex_count` 頂点の重み付き無向多重グラフについて結果を返す。個数だけをコンパイル時定数 `MOD` で割り、重みについては余りを取らない。`MOD` は 2 以上なら素数でなくてもよい。頂点数 0 のグラフと頂点数 1 のグラフには、空の全域木が 1 個あるものとする（`count_mod == 1 % MOD`）。

## 使用例

```cpp
std::vector<MinimumSpanningTreeCountEdge<long long>> edges{
    {0, 1, 2}, {0, 1, 2}, {1, 2, -1}, {0, 2, 5}
};
auto result = count_minimum_spanning_trees<1'000'000'007>(3, edges);
assert(result.connected);
assert(result.minimum_weight == 1);
assert(result.count_mod == 2);
```

## 計算量

頂点数を $N$、辺数を $M$ とする。同じ重みの辺の処理で得られる、縮約後の各連結成分の頂点数を $K_i$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_minimum_spanning_trees` | $O(M \log (M + 1) + N + \sum_i K_i^3 \log \mathrm{MOD})$ | $O(N + M + \max_i K_i^2)$ |

`MinimumSpanningTreeCountEdge` と `MinimumSpanningTreeCountResult` のフィールド参照は $O(1)$ 時間、$O(1)$ 追加領域である。

## 注意点

- `MOD >= 2` はコンパイル時に検査する。行列式は互除法による消去で計算するため、合成数の法にも対応する。
- `vertex_count` が負なら `invalid_argument`、辺の端点が $[0, N)$ の範囲外なら `out_of_range` を送出する。
- `Weight` は `Weight{}`、厳密弱順序となる `<`、加算、コピーが使える型でなければならない。
- 最小全域木の重み総和が `Weight` で表現できることは呼び出し側の前提である。
- 多重辺はそれぞれ別の辺として全域木を区別する。同じ端点・同じ重みの辺が複数あれば個数に反映される。

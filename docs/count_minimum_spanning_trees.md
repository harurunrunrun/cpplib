---
title: Count Minimum Spanning Trees (最小全域木数え上げ)
documentation_of: ../src/algorithm/graph/count_minimum_spanning_trees.hpp
---

重み付き無向多重グラフの最小全域木の重みと個数を求める。
同じ重みの辺をKruskal法でまとめて縮約し、各縮約多重グラフにMatrix-Tree定理を適用する。

## `MinimumSpanningTreeCountEdge<Weight>`

```cpp
template<class Weight>
struct MinimumSpanningTreeCountEdge {
    int from;
    int to;
    Weight weight;
};
```

無向辺の両端と重みを表す。平行辺は異なる辺として数え、自己ループは全域木の候補から除外する。

## `MinimumSpanningTreeCountResult<Weight>`

```cpp
template<class Weight>
struct MinimumSpanningTreeCountResult {
    bool connected;
    Weight minimum_weight;
    uint64_t count_mod;
};
```

- `connected`: 全域木が存在するか
- `minimum_weight`: 最小全域木の重み総和
- `count_mod`: 最小全域木の個数を `MOD` で割った余り

非連結なら `connected == false`、`minimum_weight == Weight{}`、`count_mod == 0` を返す。

## `count_minimum_spanning_trees<MOD>(vertex_count, edges)`

```cpp
template<uint64_t MOD, class Weight>
MinimumSpanningTreeCountResult<Weight> count_minimum_spanning_trees(
    int vertex_count,
    const vector<MinimumSpanningTreeCountEdge<Weight>>& edges
);
```

`vertex_count` 頂点の重み付き無向多重グラフについて結果を返す。
個数だけをコンパイル時定数 `MOD` で割り、重みには剰余を取らない。
`MOD` は2以上なら素数でなくてもよい。
頂点数0の空グラフと頂点数1のグラフには空の全域木が1個あるものとする。

## 使用例

```cpp
vector<MinimumSpanningTreeCountEdge<long long>> edges{
    {0, 1, 2}, {0, 1, 2}, {1, 2, -1}, {0, 2, 5}
};
auto result = count_minimum_spanning_trees<1'000'000'007>(3, edges);
assert(result.connected);
assert(result.minimum_weight == 1);
assert(result.count_mod == 2);
```

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。同じ重みの処理で得る各連結縮約グラフの頂点数を $K_i$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_minimum_spanning_trees` | $O(M\log(M+1)+N+\sum_i K_i^3\log(\mathtt{MOD}))$ | $O(N+M+\max_i K_i^2)$ |

`MinimumSpanningTreeCountEdge` と `MinimumSpanningTreeCountResult` のフィールド参照は $O(1)$ 時間、$O(1)$ 追加領域である。

## 注意点

- `MOD >= 2` はコンパイル時に検査する。行列式計算は合成数の法にも対応する。
- `vertex_count` が負なら `invalid_argument`、辺端点が `[0,vertex_count)` の外なら `out_of_range` を送出する。
- `Weight` は `Weight{}`、厳密弱順序の `<`、加算、コピーを利用できる必要がある。
- 最小全域木の重み総和が `Weight` で表現できることは呼び出し側の前提である。
- 平行辺は全域木の選択を区別する。同じ端点・同じ重みの辺が複数あれば個数に反映される。

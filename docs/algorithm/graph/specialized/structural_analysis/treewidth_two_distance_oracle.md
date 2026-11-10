---
title: Treewidth-Two Shortest-Path Oracle (木幅2グラフの最短路オラクル)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/treewidth_two_distance_oracle.hpp
---

基礎となる無向グラフの木幅が 2 以下である非負重み付き有向グラフについて、2 頂点間の最短距離を答えるための前処理を行う。
多重辺と自己ループを許し、辺の向きと重みはそのまま最短路に反映する。

幅 2 の木分解を、元のグラフの各頂点を一度だけ数える重み付き重心バッグで再帰的に分割する。
各階層のセパレータは高々 3 頂点である。セパレータからの距離とセパレータへの距離を、その階層の誘導部分グラフ上の Dijkstra 法で求めて保存する。
2 頂点間の最短路は、両頂点が共有する階層のいずれかのセパレータを通るため、共通の階層の距離ラベルの最小値から求められる。

## `TreewidthTwoDistanceOracle<Weight>::Edge`

```cpp
struct Edge {
    int from;
    int to;
    Weight weight;
};
```

`from` から `to` への重み `weight` の有向辺を表す。

- 時間計算量: フィールド参照は $O(1)$
- 空間計算量: $O(1)$

## `TreewidthTwoDistanceOracle`

```cpp
explicit TreewidthTwoDistanceOracle(
    int vertex_count,
    const vector<Edge>& edges,
    Weight infinity = numeric_limits<Weight>::max() / Weight{4}
);
```

頂点集合を `[0, vertex_count)` として距離オラクルを構築する。`infinity` は到達不能を表す値であり、距離の加算の飽和値でもある。正しい有限距離は `infinity` 未満でなければならない。

- 時間計算量: $O((N+M)\log(N+1)\log(M+N+1))$
- 空間計算量: $O(M+N\log N)$

$N$ は頂点数、$M$ は向きを区別した入力の辺数である。木幅が 2 以下なら、基礎となる単純無向グラフの辺数は $O(N)$ である。各階層で高々 6 回の Dijkstra 法を実行し、各頂点・各辺が属する階層の数は $O(\log N)$ である。

## `size`

```cpp
int size() const noexcept;
```

頂点数を返す。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

## `infinity`

```cpp
Weight infinity() const noexcept;
```

構築時に指定した到達不能値を返す。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

## `hierarchy_depth`

```cpp
int hierarchy_depth() const noexcept;
```

重心セパレータの階層の最大段数を返す。空グラフでは $0$ である。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

## `distance`

```cpp
Weight distance(int from, int to) const;
```

`from` から `to` への最短距離を返す。到達不能なら `infinity()` を返す。
`from == to` なら $0$ を返す。

- 時間計算量: $O(\log N)$
- 空間計算量: $O(1)$

## `reachable`

```cpp
bool reachable(int from, int to) const;
```

`from` から `to` へ到達できるとき、かつそのときに限り `true` を返す。

- 時間計算量: $O(\log N)$
- 空間計算量: $O(1)$

## 注意点

- `Weight` は比較、加算、減算、`numeric_limits` を利用できる数値型とする。
- 負の頂点数、範囲外の辺の端点、負または `infinity` 以上の辺重み、正でない `infinity`、基礎となる無向グラフの木幅が 3 以上である場合は `invalid_argument` を送出する。
- `distance` と `reachable` に範囲外の頂点を渡すと `out_of_range` を送出する。
- 距離が `infinity` 以上になる入力では、その距離を到達不能と区別できないため、十分大きな `infinity` を指定する。
- 浮動小数点型では NaN の辺重みも拒否する。

---
title: Treewidth-Two Shortest-Path Oracle (木幅2グラフ最短路オラクル)
documentation_of: ../src/algorithm/graph/treewidth_two_distance_oracle.hpp
---

基礎無向グラフの木幅が2以下である、非負重み付き有向グラフの二頂点間最短距離を前処理する。
多重辺と自己ループを許し、辺の向きと重みはそのまま最短路へ反映する。

幅2の木分解を、各元頂点を一度だけ数える重み付き重心バッグで再帰分割する。
各階層のセパレータは高々3頂点である。セパレータからの距離とセパレータへの距離を、
その階層の誘導部分グラフ上の Dijkstra 法で保存する。二頂点の最短路は、両頂点が共有する
階層のいずれかのセパレータを通るため、共通階層の距離ラベルの最小値から求められる。

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

頂点集合を `[0, vertex_count)` として距離オラクルを構築する。`infinity` は到達不能と、
距離加算の飽和値を表す。正しい有限距離は `infinity` 未満でなければならない。

- 時間計算量: $O((N+M)\log^2 N)$
- 空間計算量: $O(M+N\log N)$

$N$ は頂点数、$M$ は向きを区別した入力辺数。基礎単純無向グラフの辺数は木幅2以下なら
$O(N)$ である。各階層で高々6回の Dijkstra 法を実行し、各頂点・辺が属する階層数は
$O(\log N)$ である。

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

重心セパレータ階層の最大段数を返す。空グラフでは0である。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

## `distance`

```cpp
Weight distance(int from, int to) const;
```

`from` から `to` への最短距離を返す。到達不能なら `infinity()` を返す。
`from == to` なら0を返す。

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

`Weight` は比較、加算、減算、`numeric_limits` を利用できる数値型とする。
負の頂点数、範囲外の辺端点、負または `infinity` 以上の辺重み、非正の `infinity`、
基礎無向グラフの木幅が3以上である場合は `invalid_argument` を送出する。
`distance` と `reachable` の範囲外頂点には `out_of_range` を送出する。

距離が `infinity` 以上になる入力ではその距離を到達不能と区別できないため、十分大きな
`infinity` を指定する。浮動小数点型では NaN の辺重みも拒否する。

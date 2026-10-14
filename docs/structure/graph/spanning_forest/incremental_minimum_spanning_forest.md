---
title: Incremental Minimum Spanning Forest (逐次最小全域森) [incremental_minimum_spanning_forest]
documentation_of: ../../../../src/structure/graph/spanning_forest/incremental_minimum_spanning_forest.hpp
---

空の重み付き無向グラフへ辺を追加しながら、最小全域森を管理する。
森の辺を頂点化したLink-Cut Treeでpath上の最大重み辺を取得し、
閉路ができた場合に新しい辺より重い辺だけを置き換える。

## コンストラクタ

```cpp
IncrementalMinimumSpanningForest<
    Weight, MAX_VERTICES, MAX_EDGES
> forest(int vertex_count);
```

頂点集合を `[0, vertex_count)` とする空の森を構築する。

- 時間計算量: $O(V+M)$
- 空間計算量: $O(V+M)$

## `add_edge`

```cpp
int add_edge(int first, int second, const Weight& weight);
```

無向辺を追加する。辺番号は追加順に `0, 1, ...` とする。
新しい辺が森へ入るだけなら `-1`、閉路から取り除いた辺がある場合は
その辺番号を返す。新しい辺自身が不要な場合は、新しい辺番号を返す。

- 時間計算量: 償却 $O(\log(V+M))$
- 空間計算量: $O(1)$

## `contains_edge`

```cpp
bool contains_edge(int edge) const;
```

指定した辺が現在の最小全域森に含まれるかを返す。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

## `vertex_count`, `edge_count`, `component_count`

```cpp
int vertex_count() const;
int edge_count() const;
int component_count() const;
```

頂点数、これまでに追加した辺数、現在の連結成分数を返す。

- 時間計算量: それぞれ $O(1)$
- 空間計算量: $O(1)$

## 注意点

`Weight` は全順序を持ち、`numeric_limits<Weight>::lowest()` を利用できる
型でなければならない。重みが相異なる場合、管理される最小全域森は一意である。
同じ重みを許す場合は既に森にある辺を優先する。
頂点、辺番号、容量の範囲違反では例外を送出する。

## 容量を省略する可変長版

```cpp
IncrementalMinimumSpanningForest<long long> dynamic(vertex_count);
IncrementalMinimumSpanningForest<long long, 100000, 200000> fixed(vertex_count);
```

容量省略時は `std::vector` で頂点と追加済みの辺だけを保持し、各辺追加で Link-Cut Tree のノードを増やす。
構築は $O(V)$ 時間・空間、$E$ 本追加後の保持領域は $O(V+E)$。
`add_edge` は償却 $O(\log(V+E))$ 時間、各状態参照は $O(1)$ 時間。
容量を指定する版の固定配列も同じヘッダに残る。

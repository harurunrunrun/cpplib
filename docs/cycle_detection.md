---
title: Cycle Detection (閉路検出) [cycle_detection] [GRL_4_A]
documentation_of: ../src/algorithm/graph/traversal/cycle_detection.hpp
---

有向・無向グラフから閉路を1つ検出する。探索は反復DFSで行うため、長い一本道でも再帰スタックを消費しない。

## 型

```cpp
struct CycleDetectionResult {
    vector<int> vertices;
    vector<int> edges;
};
```

`vertices[i]` から `vertices[(i + 1) % L]` へ進む辺の入力番号が `edges[i]` である。閉路がない場合は両方とも空になる。

## 関数

## `directed_cycle_with_edges`

```cpp
CycleDetectionResult directed_cycle_with_edges(
    int n,
    const vector<pair<int, int>>& edges
);
```

`edges[id] = {from, to}` で表した有向グラフの閉路を1つ返す。多重辺と自己ループを扱える。

- 時間計算量: $O(N+M)$
- 空間計算量: 戻り値を含めて $O(N+M)$

## `directed_cycle`

```cpp
vector<int> directed_cycle(const vector<vector<int>>& graph);
```

有向隣接リストから閉路の頂点列を1つ返す。辺番号も必要な場合は `directed_cycle_with_edges` を使う。

- 時間計算量: $O(N+M)$
- 空間計算量: 戻り値と内部の辺番号付き隣接リストを含めて $O(N+M)$

## `has_directed_cycle`

```cpp
bool has_directed_cycle(const vector<vector<int>>& graph);
```

有向隣接リストに閉路が存在するかを返す。

- 時間計算量: $O(N+M)$
- 空間計算量: 内部の辺番号付き隣接リストを含めて $O(N+M)$

## `undirected_cycle_with_edges`

```cpp
CycleDetectionResult undirected_cycle_with_edges(
    int n,
    const vector<pair<int, int>>& edges
);
```

`edges[id] = {u, v}` で表した無向グラフの閉路を1つ返す。多重辺と自己ループを扱える。

- 時間計算量: $O(N+M)$
- 空間計算量: 戻り値を含めて $O(N+M)$

## `undirected_cycle`

```cpp
vector<int> undirected_cycle(
    int n,
    const vector<pair<int, int>>& edges
);
```

無向グラフの閉路の頂点列を1つ返す。辺番号も必要な場合は `undirected_cycle_with_edges` を使う。

- 時間計算量: $O(N+M)$
- 空間計算量: 戻り値を含めて $O(N+M)$

## 注意点

頂点数は非負で、すべての辺端点・隣接先は $[0,N)$ に含まれなければならない。違反時や辺数を `int` で表せない場合は `runtime_error` を送出する。返る閉路の開始位置と向きは未規定である。

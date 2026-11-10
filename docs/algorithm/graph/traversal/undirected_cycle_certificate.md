---
title: Undirected Cycle Certificate (無向閉路証明) [cycle_detection_undirected]
documentation_of: ../../../../src/algorithm/graph/traversal/undirected_cycle_certificate.hpp
---

無向グラフから、頂点列と入力の辺番号の列を対応させた閉路を 1 つ取得する。`undirected_cycle_with_edges`（`cycle_detection.hpp`）を呼び出す薄いラッパーである。

## `undirected_cycle_certificate`

```cpp
CycleDetectionResult undirected_cycle_certificate(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

閉路が存在すれば、`vertices[i]` と `vertices[(i + 1) % vertices.size()]` を結ぶ入力の辺番号を `edges[i]` に格納して返す。閉路が存在しなければ、両方の列が空の結果を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O(N+M)$。

## 空間計算量

戻り値と探索領域を含めて $O(N+M)$。

## 注意点

- 頂点番号は $[0,N)$ でなければならない。
- 自己ループと多重辺を扱える。
- 頂点数が負、辺数が `int` で表せない、または端点が範囲外の場合は `runtime_error` を送出する。
- 返される閉路の開始頂点と向きは規定しない。

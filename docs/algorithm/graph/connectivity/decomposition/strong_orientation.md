---
title: Strong Orientation (強連結向き付け)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/strong_orientation.hpp
---

無向グラフの各辺に向きを付けて、強連結な有向グラフを構成する。

## API

```cpp
optional<vector<pair<int, int>>> strong_orientation(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

強連結な向き付けが存在すれば、入力辺と同じ順序で有向辺 `(from, to)` を返す。グラフが非連結であるか橋を持つため向き付けられない場合は `std::nullopt` を返す。多重辺と自己ループを扱える。

DFS 木の辺を親から子へ、後退辺を子孫から祖先へ向ける。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- 入力検査と隣接リストの構築: $O(N + M)$
- 橋の判定と向き付け: $O(N + M)$
- `strong_orientation` 全体: $O(N + M)$

## 空間計算量

$O(N + M)$。

## 注意点

頂点番号は $[0, N)$ でなければならない。範囲外の頂点または負の頂点数を受け取ると `std::runtime_error` を送出する。頂点数 0 のグラフと頂点数 1 のグラフは向き付け可能として扱う。DFS は明示的なスタックで実装されているため、入力が深くても再帰スタックが枯渇することはない。

---
title: Chordal Graph Recognition (弦グラフ判定) [chordal_graph_recognition]
documentation_of: ../../../../../src/algorithm/graph/constraints/recognition/chordal_graph_recognition.hpp
---

単純無向グラフが弦グラフ（chordal graph）かどうかを Maximum Cardinality Search (MCS) で判定し、判定結果の証拠（certificate）を返す。

## 結果

```cpp
struct ChordalGraphRecognitionResult {
    bool chordal;
    std::vector<int> perfect_elimination_order;
    std::vector<int> induced_cycle;
};
```

- `chordal == true` のとき、`perfect_elimination_order` は完全消去順序（perfect elimination ordering）であり、`induced_cycle` は空である。
- `chordal == false` のとき、`induced_cycle` は長さ 4 以上の弦のない閉路（誘導閉路）を巡回順に並べた頂点列であり、`perfect_elimination_order` は空である。

## `chordal_graph_recognition`

```cpp
ChordalGraphRecognitionResult chordal_graph_recognition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
)
```

同じ無向辺が複数与えられた場合は 1 本にまとめて扱う。自己ループは許さない。0 頂点のグラフは弦グラフであり、空の完全消去順序を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M) \log (N + M))$。

MCS の優先度管理に（重み, 頂点番号）をキーとする二分ヒープを使い、隣接リストのソートと二分探索による隣接判定を行うため、対数因子がつく。証拠となる誘導閉路を求める BFS は高々 1 回だけ行う。

## 空間計算量

$O(N + M)$

## 注意点

`vertex_count` が負、辺の端点が範囲外、あるいは自己ループを含む場合は `runtime_error` を送出する。

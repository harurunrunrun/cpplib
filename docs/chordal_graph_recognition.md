---
title: Chordal Graph Recognition (弦グラフ判定) [chordal_graph_recognition]
documentation_of: ../src/algorithm/graph/constraints/chordal_graph_recognition.hpp
---

単純無向グラフがchordal graphかをMaximum Cardinality Searchで判定し、判定結果に対応するcertificateを返す。

## 結果

```cpp
struct ChordalGraphRecognitionResult {
    bool chordal;
    vector<int> perfect_elimination_order;
    vector<int> induced_cycle;
};
```

- `chordal == true` のとき、`perfect_elimination_order` はperfect elimination orderingであり、`induced_cycle` は空。
- `chordal == false` のとき、`induced_cycle` は長さ4以上のchordless cycleを巡回順に並べた頂点列であり、`perfect_elimination_order` は空。

## chordal graph判定

```cpp
ChordalGraphRecognitionResult chordal_graph_recognition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

同じ無向辺が複数与えられた場合は1辺にまとめて扱う。自己loopは許さない。0頂点グラフはchordalであり、空のperfect elimination orderingを返す。

## 時間計算量

- `O((N+M) log N)`

certificateとなる最短路探索は高々1回だけ行う。

## 空間計算量

- `O(N+M)`

## 注意点

`vertex_count`または辺端点が範囲外、あるいは自己loopを含む場合は `runtime_error` を送出する。

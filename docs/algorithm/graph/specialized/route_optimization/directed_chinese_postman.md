---
title: Directed Chinese Postman (有向中国人郵便配達問題)
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/directed_chinese_postman.hpp
---

非負辺重みの有向グラフで、すべての辺を少なくとも1回通って始点へ戻る閉じた歩道の最小費用を求める。

## API

```cpp
struct DirectedChinesePostmanEdge {
    int from;
    int to;
    long long cost;
};
```

有向辺と1回通る費用を表す。多重辺と自己ループを扱える。

```cpp
optional<DirectedChinesePostmanResult> directed_chinese_postman(
    int vertex_count,
    const vector<DirectedChinesePostmanEdge>& edges
)
```

辺を持つ頂点が同じ強連結成分に属さず、1本の閉じた歩道で全辺を通れない場合は `nullopt` を返す。

```cpp
struct DirectedChinesePostmanAugmentation {
    int from;
    int to;
    long long multiplicity;
    long long distance;
};
```

次数を均衡させるために、`from` から `to` への最短路を `multiplicity` 回追加することを表す。1回当たりの費用が `distance`。

```cpp
struct DirectedChinesePostmanResult {
    long long cost;
    vector<DirectedChinesePostmanAugmentation> augmentations;
};
```

- `cost`: 全辺を覆う閉じた歩道の最小費用
- `augmentations`: 最適解で追加する最短路と回数

## 時間計算量

頂点数を $N$、辺数を $M$、出次数が入次数より小さい頂点数を $S$、異なる次数不均衡頂点数を $B$ とする。

- 強連結性判定: $O(N+M)$
- $S$ 回のDijkstra法: $O(S(M+N)\log N)$
- 不均衡補正の最小費用流: 一般の逐次最短路法の上界で $O(B^2E_f+FM_f\log B)$。$F$ は補正流量、$E_f,M_f=O(B^2)$
- 全体: 上記の和

## 空間計算量

$O(N+M+SN+B^2)$。

## 注意点

辺費用は非負で、端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。費用計算には符号付き128 bit整数を使い、最終費用または返す最短距離が `long long` を超える場合は `overflow_error` を送出する。辺がないグラフの費用は0。

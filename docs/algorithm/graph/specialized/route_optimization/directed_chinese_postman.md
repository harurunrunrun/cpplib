---
title: Directed Chinese Postman (有向中国人郵便配達問題)
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/directed_chinese_postman.hpp
---

非負の辺重みを持つ有向グラフで、すべての辺を少なくとも 1 回ずつ通って始点へ戻る閉じた歩道の最小費用を求める。

## API

```cpp
struct DirectedChinesePostmanEdge {
    int from;
    int to;
    long long cost;
};
```

有向辺と、それを 1 回通る費用を表す。多重辺と自己ループを扱える。

```cpp
optional<DirectedChinesePostmanResult> directed_chinese_postman(
    int vertex_count,
    const vector<DirectedChinesePostmanEdge>& edges
)
```

辺を持つ頂点が 1 つの強連結成分に収まらず、1 本の閉じた歩道で全辺を通れない場合は `nullopt` を返す。

```cpp
struct DirectedChinesePostmanAugmentation {
    int from;
    int to;
    long long multiplicity;
    long long distance;
};
```

入次数と出次数を均衡させるために、`from` から `to` への最短路を `multiplicity` 回追加することを表す。1 回あたりの費用は `distance` である。

```cpp
struct DirectedChinesePostmanResult {
    long long cost;
    vector<DirectedChinesePostmanAugmentation> augmentations;
};
```

- `cost`: 全辺を通る閉じた歩道の最小費用
- `augmentations`: 最適解で追加する最短路とその回数

## 時間計算量

頂点数を $N$、辺数を $M$、出次数が入次数より小さい頂点の個数を $S$、入次数と出次数が異なる頂点の個数を $B$ とする。

- 強連結性の判定: $O(N+M)$
- $S$ 回の Dijkstra 法: $O(S(M+N)\log(M+N+1))$
- 不均衡を補正する最小費用流: 頂点数 $B+2$、辺数 $O(B^2)$ のネットワーク上で $O(B^3+AB^2\log B)$。$A$ は増加路の本数で、補正流量を $F$ とすると $A\le F$
- 全体: 上記の和

## 空間計算量

$O(N+M+SN+B^2)$。

## 注意点

- 辺の費用は非負で、端点は $[0,N)$ でなければならない。頂点数が負の場合も含め、違反時は `runtime_error` を送出する。
- 費用の計算には符号付き 128 bit 整数を使い、最終的な費用または返す最短距離が `long long` に収まらない場合は `overflow_error` を送出する。
- 辺がないグラフの費用は $0$ である。

---
title: Counting C4 (長さ4の閉路数え上げ) [counting_c4]
documentation_of: ../src/algorithm/graph/specialized/counting_c4.hpp
---

自己loopを含まない無向多重グラフで、4つの異なる頂点を巡る長さ4のcycleを数える。多重辺はラベル付きの別々の辺として区別する。

## 各辺を含むC4数

```cpp
vector<long long> count_c4_per_edge(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

`answer[e]` は、辺 `e` と他の相異なる3辺を選んでC4になる選び方の数。平行辺を圧縮して重み付き単純グラフにし、次数順orientation上の長さ2のpathをまとめて数える。

## 時間計算量

- `O(N + M log M + M sqrt(M))`

平行辺がなく、端点pairごとにまとめられた入力では圧縮後の本体は `O(N+M sqrt(M))`。

## 空間計算量

- `O(N+M)`

## C4総数

```cpp
__int128_t count_c4(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

C4の総数を返す。各C4は構成する4辺の各answerに1回ずつ現れるため、`count_c4_per_edge` の総和を4で割った値である。

## 時間計算量

- `O(N + M log M + M sqrt(M))`

## 空間計算量

- `O(N+M)`

## 注意点

各辺の答えは signed 64-bit 整数で返し、総数は多重辺で signed 64-bit の範囲を超え得るため __int128_t で返す。範囲違反または自己loopを含む場合は runtime_error を送出する。

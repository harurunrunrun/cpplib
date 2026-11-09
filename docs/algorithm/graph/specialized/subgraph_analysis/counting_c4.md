---
title: Counting C4 (長さ4の閉路数え上げ) [counting_c4]
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/counting_c4.hpp
---

自己ループを含まない無向多重グラフで、相異なる 4 頂点を巡る長さ 4 の閉路（C4）を数える。多重辺はラベル付きの別々の辺として区別する。

## 各辺を含む C4 の個数

```cpp
vector<long long> count_c4_per_edge(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

`answer[e]` は、辺 `e` と、ほかの相異なる 3 辺を選んで C4 を作る選び方の数である。平行辺を圧縮して重み付き単純グラフにし、次数順に向き付けたグラフ上で長さ 2 のパスをまとめて数える。

### 時間計算量

- $O(N\log N + M\log M + M\sqrt{M})$

$N$ は頂点数、$M$ は辺数である。圧縮後の本体の計算は $O(N+M\sqrt{M})$ で、残りは辺のソートと頂点の次数順ソートによる。

### 空間計算量

- $O(N+M)$

## C4 の総数

```cpp
__int128_t count_c4(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

C4 の総数を返す。各 C4 はそれを構成する 4 辺の答えに 1 回ずつ現れるため、`count_c4_per_edge` の総和を $4$ で割った値である。

### 時間計算量

- $O(N\log N + M\log M + M\sqrt{M})$

### 空間計算量

- $O(N+M)$

## 注意点

- 各辺の答えは符号付き 64 bit 整数で返す。総数は多重辺があると符号付き 64 bit 整数の範囲を超え得るため、`__int128_t` で返す。
- 頂点数が負、端点が範囲外、または自己ループを含む場合は `runtime_error` を送出する。

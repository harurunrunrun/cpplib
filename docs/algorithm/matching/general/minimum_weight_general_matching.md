---
title: Minimum-Weight Maximum-Cardinality Matching in General Graphs (一般グラフの最小重み最大マッチング)
documentation_of: ../../../../src/algorithm/matching/general/minimum_weight_general_matching.hpp
---

一般グラフの最小重み最大マッチングを求める。まずマッチングの辺数を最大化し、その中で重みの和を最小化する。Edmonds の blossom 法を主双対法で重み付きに拡張したものを用いる。

## 関数

```cpp
minimum_weight_general_matching<T>(n, edges, inf = numeric_limits<T>::max() / 4)
```

`edges` は `MinimumWeightGeneralMatchingEdge<T>{from, to, cost}` の列である。自己ループは無視する。多重辺がある場合は最小コストの辺を使う。`T` は `bool` 以外の 64 ビット以下の整数型でなければならない。`inf` は以前の API との互換性のために残している引数で、使われない。

`MinimumWeightGeneralMatchingResult<T>` は次のメンバを持つ。

```cpp
int size;
T cost;
vector<int> match;
```

`match[v]` は `v` と対応する頂点、未マッチなら `-1` である。

## 時間計算量

- 多重辺の圧縮と密なコスト表の構築: $O(N^2 + M)$
- `minimum_weight_general_matching`: $O(N^3 + M)$。長さ $N$ の `match` と重みの和の構築を含む

## 空間計算量

- 密なコスト表、blossom の作業領域、入力辺を含めて $O(N^2 + M)$

## 注意点

頂点数は $0$ 以上 `INT_MAX / 2` 以下、各端点は頂点範囲内でなければならず、違反時や内部の辺番号が範囲を超える場合は `runtime_error` を送出する。重みの和が `T` に収まらない場合は `std::overflow_error` を送出する。記載した計算量には引数検査と戻り値の構築を含む。

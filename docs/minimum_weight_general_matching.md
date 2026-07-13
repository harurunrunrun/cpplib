---
title: Minimum Weight General Matching
documentation_of: ../src/algorithm/matching/minimum_weight_general_matching.hpp
---

一般グラフの最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。Edmondsのblossom algorithmをprimal-dual法で重み付きへ拡張している。

# 関数

```cpp
minimum_weight_general_matching<T>(n, edges, inf)
```

`edges` は `MinimumWeightGeneralMatchingEdge<T>{from, to, cost}` の列。自己ループは無視する。多重辺がある場合は最小コストの辺を使う。`T` は64bit以下の整数型。

`MinimumWeightGeneralMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> match;
```

`match[v]` は `v` と対応する頂点、未マッチなら `-1`。重み和が `T` に収まらない場合は `std::overflow_error` を送出する。`inf` は以前のAPIとの互換性のために残している。

## 時間計算量

- 多重辺圧縮とdense cost table構築: $O(N^2+M)$
- `minimum_weight_general_matching`: $O(N^3+M)$。長さ $N$ の `match` と重み和の構築を含む

## 空間計算量

- dense table、blossom作業領域、入力辺を含めて $O(N^2+M)$

## API契約・前提・例外

`match[v]` は対応頂点、未matchingなら `-1`。`T` はbool以外の64bit以下整数型。self-loopを無視し、多重辺は最小costを使う。範囲・内部辺id超過で `runtime_error`、結果costがT外なら `overflow_error`。`inf` は互換引数。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。

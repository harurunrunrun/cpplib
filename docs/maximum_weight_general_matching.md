---
title: Maximum-Weight Matching in General Graphs (一般グラフの最大重みマッチング) [general_weighted_matching]
documentation_of: ../src/algorithm/matching/maximum_weight_general_matching.hpp
---

一般グラフ上で辺重みの総和が最大となるマッチングを、重み付きEdmondsのblossom algorithmで求める。マッチングの辺数は優先しない。

# 型

```cpp
template<class T>
struct MaximumWeightGeneralMatchingEdge {
    int from;
    int to;
    T weight;
};

template<class T>
struct MaximumWeightGeneralMatchingResult {
    int size;
    T weight;
    vector<int> match;
};
```

`size` は選ばれた辺数、`weight` は重み和である。`match[v]` は対応する頂点で、未マッチなら `-1` になる。

# 関数

## `maximum_weight_general_matching`

```cpp
template<class T>
MaximumWeightGeneralMatchingResult<T> maximum_weight_general_matching(
    int n,
    const vector<MaximumWeightGeneralMatchingEdge<T>>& edges
);
```

自己ループを無視し、同じ2頂点間に複数の辺がある場合は最大重みだけを使う。負辺しかない頂点は未マッチのままにできる。`T` は `bool` 以外の64bit以下の整数型でなければならない。

- 時間計算量: 多重辺の圧縮を含めて $O(N^3+M)$
- 空間計算量: denseな重み表とblossom作業領域を含めて $O(N^2+M)$

# 注意点

これは「重み和」を最優先するAPIであり、「辺数を最大化してから重みを最適化する」`minimum_weight_general_matching` とは目的関数が異なる。頂点数は非負で、すべての端点は $[0,N)$ に含まれなければならない。違反時は `runtime_error`、重み和が `T` に収まらない場合は `overflow_error` を送出する。

---
title: Sparse Undirected Component Count (疎頂点無向グラフの連結成分数) [FOXLINGS]
documentation_of: ../src/algorithm/graph/sparse_undirected_component_count.hpp
---

頂点数に対して辺に現れる頂点が少ない無向グラフの連結成分数を、出現頂点だけを保持して求める。

## sparse_undirected_component_count

```cpp
long long sparse_undirected_component_count(
    long long vertex_count,
    const vector<pair<long long, long long>>& edges
);
```

## 引数

- `vertex_count`: 頂点数。頂点番号は `long long` で扱う。
- `edges`: 無向辺列。

## 戻り値

孤立頂点を含むグラフ全体の連結成分数。

## API別の時間計算量・空間計算量

辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sparse_undirected_component_count` | $O(M\log(M+1))$ | $O(M)$ |

## 注意点

- `vertex_count` は非負、各端点は $[0,\mathtt{vertex\_count})$ でなければならない。違反時は `runtime_error` を送出する。
- 自己ループと多重辺は連結成分数を変えない。
- $O(N)$ の頂点配列を確保しないため、`vertex_count` が大きくても辺数に比例するメモリだけを使用する。

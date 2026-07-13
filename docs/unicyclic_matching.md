---
title: Unicyclic Matching (単一閉路グラフのマッチング)
documentation_of: ../src/algorithm/matching/unicyclic_matching.hpp
---

各連結成分が木または $N$ 頂点 $N$ 辺の無向グラフである場合の最大マッチング。

# 関数

```cpp
unicyclic_matching(n, edges)
```

`edges` は `UnicyclicMatchingEdge{from, to}` の列。

`UnicyclicMatchingResult` は次を持つ。

```cpp
int size;
vector<int> match;
```

自己ループは扱わない。多重辺は扱える。

## 時間計算量

- `unicyclic_matching`: $O(N+M)$

各連結成分の検査、木DP・cycle DP、長さ $N$ の `match` 出力を含む。

## 空間計算量

- 戻り値と作業領域を含めて $O(N+M)$

## API契約・前提・例外

`match[v]` は対応頂点、未matchingなら `-1`。各連結成分は木または閉路を1つだけ持つgraphとする。多重辺を許しself-loopは不可。条件違反で `runtime_error`。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。

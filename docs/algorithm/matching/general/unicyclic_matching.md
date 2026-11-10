---
title: Unicyclic Matching (単一閉路グラフのマッチング)
documentation_of: ../../../../src/algorithm/matching/general/unicyclic_matching.hpp
---

各連結成分が木、または頂点数と辺数が等しい（閉路をちょうど 1 つ持つ）無向グラフである場合に、最大マッチングを求める。

## 関数

```cpp
unicyclic_matching(n, edges)
```

`edges` は `UnicyclicMatchingEdge{from, to}` の列である。

`UnicyclicMatchingResult` は次のメンバを持つ。

```cpp
int size;
vector<int> match;
```

`match[v]` は `v` と対応する頂点、未マッチなら `-1` である。

## 時間計算量

- `unicyclic_matching`: $O(N + M)$

各連結成分の検査、木 DP・閉路 DP、長さ $N$ の `match` の出力を含む。

## 空間計算量

- 戻り値と作業領域を含めて $O(N + M)$

## 注意点

多重辺は扱える（長さ 2 の閉路とみなす）。自己ループや、木でも単一閉路グラフでもない成分を含む場合は `runtime_error` を送出する。

頂点数は非負、各端点は頂点範囲内でなければならず、違反時は `runtime_error` を送出する。記載した計算量には引数検査と戻り値の構築を含む。

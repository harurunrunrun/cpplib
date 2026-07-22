---
title: General Matching (一般グラフのマッチング) [general_matching]
documentation_of: ../src/algorithm/matching/general/general_matching.hpp
---

一般グラフの最大マッチング。

## コンストラクタ

```cpp
GeneralMatching graph(n)
```

## 関数

```cpp
graph.add_edge(u, v)
graph.solve()
```

`GeneralMatchingResult` は次を持つ。

```cpp
int size;
vector<int> match;
```

## 時間計算量

$N$ を頂点数、$M$ を追加済み無向辺数とする。

- `GeneralMatching(n)`: $O(N)$
- `add_edge`: 償却 $O(1)$
- `lca`, `mark_path`: $O(N)$
- `find_path`: worst-case $O(M+N^2)$
- `solve`: worst-case $O(NM+N^3)$。長さ $N$ の `match` 出力を含む

単純グラフでは $M=O(N^2)$ なので通常の $O(N^3)$ boundになる。多重辺を重複追加した場合はその走査時間 $O(NM)$ を含む。

## 空間計算量

- graph、blossom作業領域、戻り値を含めて $O(N+M)$

## 注意点

`match[v]` は対応頂点または `-1`。public `lca`, `mark_path`, `find_path` はblossom探索helperで、`find_path` が作る内部状態を前提とする。通常は `solve` を使う。self-loopは無視し、多重辺を許す。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。

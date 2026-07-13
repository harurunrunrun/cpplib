---
title: General Matching
documentation_of: ../src/algorithm/matching/general_matching.hpp
---

一般グラフの最大マッチング。

# コンストラクタ

```cpp
GeneralMatching graph(n)
```

# 関数

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

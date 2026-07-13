---
title: Lowlink
documentation_of: ../src/algorithm/graph/lowlink.hpp
---

無向グラフの関節点と橋を求める。

# 関数

```cpp
lowlink(n, edges)
```

`edges` は無向辺の列。多重辺を扱える。

`LowlinkResult` は次を持つ。

```cpp
vector<int> ord;
vector<int> low;
vector<int> articulation;
vector<int> bridges;
```

`bridges` には入力辺の id が入る。

## 時間計算量

$A$ を関節点数、$B$ を橋数とする。

- DFSと隣接リスト構築: $O(N+M)$
- 出力IDの整列: $O(A\log(A+1)+B\log(B+1))$
- `lowlink` 全体: $O(N+M+A\log(A+1)+B\log(B+1))$

## 空間計算量

- 戻り値と隣接リストを含めて $O(N+M)$

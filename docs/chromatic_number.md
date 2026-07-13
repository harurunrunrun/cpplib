---
title: Chromatic Number
documentation_of: ../src/algorithm/graph/chromatic_number.hpp
---

無向グラフの彩色数を求める。

# 関数

```cpp
chromatic_number(graph)
```

`graph[v]` に隣接頂点を入れる。無向グラフである必要がある。

実装上 `N <= 24`。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数とする。

- 入力検査と独立集合tableの前処理: $O(N^2+M+2^N)$
- subset DP: $O(3^N)$
- `chromatic_number` 全体: $O(3^N+N^2+M)$

戻り値の構築は $O(1)$。

## 空間計算量

- $O(2^N+N)$

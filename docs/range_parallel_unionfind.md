---
title: Range Parallel Union-Find (区間並列Union-Find)
documentation_of: ../src/structure/dsu/range_parallel_unionfind.hpp
---

同じ長さの2区間を位置ごとに対応させ、対応する頂点をUnionfindで併合する。長さ$2^k$の区間ごとに補助Unionfindを持ち、一度処理した区間対を再帰しない。

# テンプレート引数・構築

```cpp
RangeParallelUnionFind<MAX_SIZE> dsu(n)
```

`MAX_SIZE`は頂点数の上限、`n`は実際の頂点数。

## 時間計算量

- $O(N\log N)$

# 1組の併合

```cpp
int merge(int u, int v)
int merge(int u, int v, MergeCallback callback)
```

頂点`u`, `v`を併合し、新しく起きた頂点成分の併合回数を返す。実際に異なる成分を併合したとき、`callback(new_root, absorbed_root)`を1回呼ぶ。callback呼び出し時点で`new_root`がleaderになっている。

# 区間のparallel併合

```cpp
int merge_ranges(int l1, int r1, int l2, int r2)
int merge_ranges(int l1, int r1, int l2, int r2, MergeCallback callback)
```

`r1-l1 == r2-l2`を要求し、各`0 <= i < r1-l1`について`l1+i`と`l2+i`を併合する。返り値とcallbackは`merge`と同じ。

## 時間計算量

$Q$回の`merge`・`merge_ranges`全体で

$$O((N\log N+Q)\alpha(N))$$

1回の操作では新しく展開される補助区間数に比例する。

# 検索

```cpp
int leader(int v)
bool same(int u, int v)
int component_size(int v)
int groups() const
int size() const
```

## 時間計算量

- `leader`, `same`, `component_size`: amortized $O(\alpha(N))$
- `groups`, `size`: $O(1)$

# 空間計算量

- $O(N\log N)$

区間は半開区間で指定する。範囲外の頂点、区間、長さの異なる区間を渡すと例外を送出する。

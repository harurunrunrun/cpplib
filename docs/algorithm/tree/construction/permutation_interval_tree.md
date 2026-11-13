---
title: Permutation Strong Interval Tree (順列の強共通区間分解木) [common_interval_decomposition_tree]
documentation_of: ../../../../src/algorithm/tree/construction/permutation_interval_tree.hpp
---

$[0, N)$ の順列の強共通区間分解木を構築する。

## API

```cpp
explicit PermutationIntervalTree(const std::vector<int>& permutation)
int root() const
const std::vector<Node>& nodes() const
int smallest_container(int l, int r) const
```

- `root()` は根のノード番号を返す。空の入力なら `-1` を返す。
- `nodes()` はノードの配列を返す。各ノードは位置の半開区間 `left`, `right`、値の半開区間 `minimum`, `maximum`、線形ノードかを表す `linear`、位置の順に並んだ `children` を持つ。
- `smallest_container(l, r)` は `[l, r)` を含む最小の強共通区間のノード番号を返す。

単調スタックと区間加算・最大値のセグメント木で分解し、包含クエリにはダブリングを使う。

## 時間計算量

構築は $O(N\log(N+1))$、包含クエリは $O(\log(N+1))$、保持する領域は $O(N\log(N+1))$ である。

## 注意点

- 順列でない入力では `std::invalid_argument` を送出する。
- 包含クエリは `0 <= l < r <= N` を満たす必要があり、違反時は `std::out_of_range` を送出する。
- 葉も `linear == true` である。ノード番号は構築順で、根が `0` とは限らない。

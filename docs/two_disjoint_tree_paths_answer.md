---
title: Two Disjoint Tree Paths Answer (頂点非共有2パス回答) [TWOPATHS]
documentation_of: ../src/algorithm/tree/two_disjoint_tree_paths_answer.hpp
---

木に含まれる頂点非共有な2本のpathについて、辺数の積の最大値と証明情報を返す。

## API

### `two_disjoint_tree_paths_answer<MAX_SIZE>(vertex_count, edges)`

`TreeTwoDisjointPathsResult` を返す。`product` が最大積、残りのfieldが選ばれた切断辺と両成分の直径である。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(MAX_SIZE+N)$

## 注意点

- `0 < vertex_count <= MAX_SIZE`、辺数は `vertex_count-1` でなければならない。
- self-loop、多重辺、cycle、非連結、範囲外頂点では `runtime_error`。
- `N=1` では積0、切断辺indexと端点は `-1` を返す。

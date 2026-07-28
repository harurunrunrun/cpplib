---
title: Ladder Decomposition (ラダー分解)
documentation_of: ../../../../src/algorithm/tree/decomposition/ladder_decomposition.hpp
---

各頂点から最大部分木を持つ子へ伸びる重いパスをラダーとして分解する。

## API

- `LadderDecomposition(graph, root)`: 分解を構築する。
- `size()`, `ladder_count()`: 頂点数、ラダー数を返す。
- `parent(v)`, `depth(v)`: 親、深さを返す。
- `ladder_id(v)`, `position(v)`, `head(v)`: 所属ラダー、位置、先頭を返す。
- `ladder_vertices(id)`: ラダー上の頂点を根側から返す。
- `path_to_root(v)`: 根までをラダー区間列に分解する。

## 時間計算量

- 構築: $O(N)$。
- 各単一点取得: $O(1)$。
- `path_to_root`: $O(\log N)$。

## 注意点

- 入力は連結な無向木とする。

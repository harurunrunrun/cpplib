---
title: Ladder Decomposition (ラダー分解)
documentation_of: ../../../../src/algorithm/tree/decomposition/ladder_decomposition.hpp
---

各頂点から部分木サイズが最大の子へ伸びる heavy path をラダーとして、木を分解する。

## API

- `explicit LadderDecomposition(graph, root = 0)`: 隣接リスト `graph` と根 `root` から分解を構築する。デフォルトコンストラクタは空の分解を作る。
- `size()`, `ladder_count()`: 頂点数とラダー数を返す。
- `parent(v)`, `depth(v)`: 親と深さを返す。根の親は根自身である。
- `ladder_id(v)`, `position(v)`, `head(v)`: 所属するラダーの番号、ラダー内の位置（根側から 0-indexed）、ラダーの先頭の頂点を返す。
- `ladder_vertices(id)`: ラダー上の頂点を根側から順に並べた列（const 参照）を返す。
- `path_to_root(v)`: `v` から根までのパスを、`(ラダー番号, そのラダーの先頭から使う頂点数)` の組の列に分解して返す。`v` 側から順に並ぶ。

## 時間計算量

- 構築: $O(N)$
- 各単一点の取得: $O(1)$
- `path_to_root`: $O(\log N)$

## 注意点

- 入力は連結な無向木とする。
- `root`、辺の端点、頂点、ラダー番号が範囲外の場合は `std::out_of_range` を、グラフが非連結の場合は `std::invalid_argument` を送出する。閉路は検出しない。

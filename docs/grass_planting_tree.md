---
title: Grass Planting Tree (木上パス加算・辺取得) [GRASSPLA]
documentation_of: ../src/algorithm/tree/grass_planting_tree.hpp
---

根付き木のパスに1を加え、指定した木辺の現在値を取得する専用構造である。

## API

### `GrassPlantingTree<MAX_SIZE>(n)`

頂点数 `n` の空の木を作る。

- 時間計算量: $O(n)$

### `size()`

頂点数を返す。時間計算量は $O(1)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

木を根付き化する。時間計算量は $O(n)$。

### `plant_path(left, right)`

パス上の全辺へ1を加える。時間計算量は $O(\log n)$。

### `edge_plant_count(left, right)`

隣接する2頂点を結ぶ辺の値を返す。時間計算量は $O(1)$。

## 注意点

- `MAX_SIZE > 0`、`0 <= n <= MAX_SIZE` が必要である。
- `build` 前に連結な木を構成する。`build` 前のqueryは `runtime_error`。
- `edge_plant_count` の2頂点は木辺の両端でなければならない。

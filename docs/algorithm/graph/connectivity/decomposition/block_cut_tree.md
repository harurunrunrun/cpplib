---
title: Block-cut Tree (ブロックカット木)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/block_cut_tree.hpp
---

無向グラフを、二重頂点連結成分（ブロック）ごとのブロックノードと元の頂点からなる森へ変換する。
元の頂点のノード番号は $[0, N)$（$N$ は `original_vertex_count`）、ブロック `b` のノード番号は `original_vertex_count + b` である。各ブロックノードと、そのブロックに属するすべての元の頂点とを辺で結ぶ。

```cpp
auto result = block_cut_tree(n, edges);
```

## `BlockCutTreeResult`

- `original_vertex_count`: 入力の頂点数。
- `block_count`: ブロック数。孤立点も 1 つのブロックになる。
- `articulation`: 関節点を昇順に保持する。
- `block_vertices[b]`: ブロック `b` に属する元の頂点の列（昇順）。
- `block_edges[b]`: ブロック `b` に属する入力辺の ID の列（昇順）。
- `tree`: 元の頂点とブロックノードを結ぶ森の隣接リスト。
- `vertex_blocks[v]`: 元の頂点 `v` が属するブロック ID の列。

## API

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `block_cut_tree(n, edges)` | ブロックカット森を構築する | $O(N + M \log (M + 1) + A \log (A + 1))$ |
| `node_count()` | `tree.size()` を返す | $O(1)$ |
| `is_original_vertex(node)` | 元の頂点のノードか判定する | $O(1)$ |
| `is_block_node(node)` | ブロックノードか判定する | $O(1)$ |
| `block_node(block)` | ブロック ID からノード番号へ変換する | $O(1)$ |
| `block_id(node)` | ブロックノードからブロック ID へ変換する | $O(1)$ |

ここで $M$ は辺数、$A$ は関節点数である。戻り値を含む空間計算量は $O(N + M)$。

## 注意点

- `vertex_count` は非負、入力辺の端点は $[0, n)$ でなければならない。
- 多重辺は扱えるが、自己ループは扱えない。
- `vertex_count` が負、端点が範囲外、または自己ループを含む場合は `std::runtime_error` を送出する。
- `block_node` と `block_id` は、引数が範囲外のとき `std::runtime_error` を送出する。
- 内部で使う二重頂点連結成分分解の DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。

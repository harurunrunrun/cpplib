---
title: Block-cut Tree (ブロックカット木)
documentation_of: ../src/algorithm/graph/connectivity/block_cut_tree.hpp
---

無向グラフを二重頂点連結成分ごとのblock nodeと元頂点からなるforestへ変換する。
元頂点は `[0, original_vertex_count)`、block `b` のnodeは
`original_vertex_count + b`。各block nodeと、その成分に属するすべての元頂点を結ぶ。

```cpp
auto result = block_cut_tree(n, edges);
```

## `BlockCutTreeResult`

- `original_vertex_count`: 入力頂点数。
- `block_count`: block数。孤立点も1つのblockになる。
- `articulation`: 関節点を昇順で保持する。
- `block_vertices[b]`: block `b` の元頂点列。
- `block_edges[b]`: block `b` の入力辺id列。
- `tree`: 元頂点とblock nodeを結ぶforestの隣接リスト。
- `vertex_blocks[v]`: 元頂点 `v` が属するblock id列。

## API

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `block_cut_tree(n, edges)` | block-cut forestを構築する | $O(N+M\log(M+1))$ |
| `node_count()` | `tree.size()`を返す | $O(1)$ |
| `is_original_vertex(node)` | 元頂点nodeか判定する | $O(1)$ |
| `is_block_node(node)` | block nodeか判定する | $O(1)$ |
| `block_node(block)` | block idからnode番号へ変換する | $O(1)$ |
| `block_id(node)` | block nodeからblock idへ変換する | $O(1)$ |

`block_node`と`block_id`は範囲外で`runtime_error`を送出する。
入力辺の端点は`[0,n)`でなければならない。並列辺は扱えるが、自己loopは例外。
返り値を含む空間計算量は $O(N+M)$。

## 注意点

頂点番号は $[0,N)$ に収め、辺の向き、多重辺、自己loop、連結性は各APIで記した入力条件に従う。戻り値の頂点番号も同じ0-indexedである。

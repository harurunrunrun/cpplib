---
title: Auxiliary Tree (補助木)
documentation_of: ../src/algorithm/tree/auxiliary_tree.hpp
---

指定した頂点と、それらのLCAだけを残して木を圧縮する。
Virtual Tree、圧縮木とも呼ばれる。元の木の辺数を数える必要がある場合は
`edge_length(index)` を使う。

```cpp
AuxiliaryTree<MAX_N> tree(n);
tree.add_edge(u, v);
tree.build(root);

int m = tree.compress(vertices);
for(int i = 0; i < m; ++i){
    int v = tree.vertex(i);
    int p = tree.parent_index(i);
    bool specified = tree.selected(i);
}
```

圧縮後の頂点は元の木のDFS順に並ぶ。`parent_index(i)` は圧縮後の配列に
おける親の添字であり、根では `-1`。空集合を圧縮した場合は頂点数0となる。
指定頂点の重複は除去される。

## API別の時間計算量・空間計算量

元の木の頂点数を $N$、`compress` に渡した頂点数を $K$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| 構築 | $O(1)$ |
| `size()`, `edge_count()` | $O(1)$ |
| `add_edge(u, v)` | $O(1)$ |
| `build(root)` | $O(N\log N)$ |
| `depth(v)`, `tin(v)` | $O(1)$ |
| `is_ancestor(u, v)` | $O(1)$ |
| `lca(u, v)`, `dist(u, v)` | $O(\log N)$ |
| `compress(vertices)` | $O(K\log K+K\log N)$ |
| `compressed_size()` | $O(1)$ |
| `vertex(i)`, `parent_index(i)`, `edge_length(i)`, `selected(i)` | $O(1)$ |

前処理後の保存領域は $O(MAX\_SIZE\log MAX\_SIZE)$。圧縮木の頂点数は
高々 $2K-1$ で、問い合わせごとの動的確保は行わない。

## 参考

- [ABC340 G公式解説](https://atcoder.jp/contests/abc340/editorial/9256)

## 注意点

`VirtualTree<MAX_SIZE>` はalias。`compress` は圧縮後sizeを返す。`vertex`, `parent_index`, `edge_length`, `selected` は元頂点、圧縮上の親、元木上距離、指定頂点flagを返す。size・辺・圧縮容量を検査し、違反時は `runtime_error`。

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。

---
title: Auxiliary Tree (補助木)
documentation_of: ../../../../src/algorithm/tree/decomposition/auxiliary_tree.hpp
---

指定した頂点と、それらの LCA だけを残して木を圧縮する。
Virtual Tree、圧縮木とも呼ばれる。元の木での辺数が必要な場合は `edge_length(index)` を使う。

## 使い方

```cpp
AuxiliaryTree<> tree(n);          // 可変長版
// AuxiliaryTree<100000> tree(n); // 固定容量版
tree.add_edge(u, v);
tree.build(root);

int m = tree.compress(vertices);
for(int i = 0; i < m; ++i){
    int v = tree.vertex(i);
    int p = tree.parent_index(i);
    bool specified = tree.selected(i);
}
```

テンプレート引数 `MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使い、問い合わせごとの動的確保を行わない。
`VirtualTree<MAX_SIZE>` は `AuxiliaryTree<MAX_SIZE>` の別名である。

## API

- `explicit AuxiliaryTree(int size)`: 頂点数 `size` の空の木を作る。`size` のデフォルトは固定容量版では `MAX_SIZE`、可変長版では `0` である。
- `size()`, `edge_count()`: 頂点数と、追加済みの辺数を返す。
- `add_edge(u, v)`: 無向辺を追加する。
- `build(root)`: `root` を根として深さ・DFS 順・ダブリング表を構築する。
- `depth(v)`, `tin(v)`, `is_ancestor(u, v)`, `lca(u, v)`, `dist(u, v)`: 元の木での深さ、DFS の訪問時刻、祖先判定（同じ頂点も祖先とみなす）、LCA、距離を返す。
- `compress(vertices)`: `vertices`（`std::span<const int>`）を圧縮し、圧縮後の頂点数を返す。指定頂点の重複は除去される。空集合を圧縮した場合は頂点数 $0$ となる。
- `compressed_size()`: 直近の `compress` の結果の頂点数を返す。
- `vertex(i)`, `parent_index(i)`, `edge_length(i)`, `selected(i)`: 圧縮後の $i$ 番目の頂点について、元の頂点番号、圧縮木での親の添字（根では `-1`）、親との元の木での距離、指定された頂点かどうかを返す。

圧縮後の頂点は元の木の DFS 順に並ぶ。

## API 別の時間計算量・空間計算量

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

前処理後の保持領域は、可変長版で $O(N\log N)$、固定容量版で $O(\texttt{MAX_SIZE} \log \texttt{MAX_SIZE})$ である。
圧縮木の頂点数は高々 $2K-1$ である。ダブリングの段数は、可変長版では実行時に算出する。

## 注意点

- 頂点と `root` は範囲内でなければならない。入力は無向の連結木とする。
- 頂点数・辺数・圧縮の容量の超過、端点の範囲外、閉路、連結性の違反、`build` 前の問い合わせ、範囲外の圧縮後添字では `std::runtime_error` を送出する。表のサイズが `int` の範囲を超える場合は `std::length_error` を送出する。
- 記載した計算量には木の検査を含む。

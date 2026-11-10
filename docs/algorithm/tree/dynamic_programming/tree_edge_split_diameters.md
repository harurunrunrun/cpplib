---
title: Tree Edge Split Diameters (辺切断後の木の直径)
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/tree_edge_split_diameters.hpp
---

入力順に与えた木の各辺を 1 本ずつ削除し、分かれた 2 つの成分の直径を求める。
直径は辺数で測り、1 頂点だけの成分の直径は $0$ とする。

## API

### `TreeEdgeSplitDiameter`

```cpp
struct TreeEdgeSplitDiameter{
    int first_diameter;
    int second_diameter;
};
```

`first_diameter` は `edges[i].first` を含む成分の直径、`second_diameter` は `edges[i].second` を含む成分の直径である。
各メンバの参照は $O(1)$ である。

### `tree_edge_split_diameters<MAX_SIZE>(vertex_count, edges)`

```cpp
template<int MAX_SIZE = -1>
std::vector<TreeEdgeSplitDiameter> tree_edge_split_diameters(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
);
```

戻り値の `i` 番目は、`edges[i]` を削除した結果に対応する。
入力の辺の向きを反転すると、対応する 2 つの直径も入れ替わる。

`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` で作業領域を確保する。明示すると固定容量の作業領域を使う。

```cpp
auto result = tree_edge_split_diameters(vertex_count, edges);
auto fixed = tree_edge_split_diameters<100000>(vertex_count, edges);
```

## 時間計算量

$N$ を `vertex_count` とする。

- 関数全体: $O(N)$
- 戻り値の各メンバの参照: $O(1)$

CSR 形式の隣接列の構築、木の検査、下向きの DP、上向きの DP、長さ $N-1$ の戻り値の構築を含む。再帰は使用しない。
使用メモリは、作業領域が可変長版で $O(N)$、固定容量版で $O(\texttt{MAX_SIZE})$、戻り値が $O(N)$ である。

## 注意点

- `0 < vertex_count` が必要で、固定容量版では `vertex_count <= MAX_SIZE` も必要である。`MAX_SIZE` は `-1` または正の値でなければならない（`static_assert`）。
- `edges` はちょうど `vertex_count - 1` 本で、頂点番号は `[0, vertex_count)`、グラフは連結かつ閉路なしでなければならない。
- 自己ループ、多重辺、閉路、非連結、辺数の不一致、範囲外の頂点、容量超過では `std::runtime_error` を送出する。`vertex_count` が `INT_MAX / 2` を超える場合は `std::length_error` を送出する。
- `vertex_count == 1` では空の `std::vector` を返す。

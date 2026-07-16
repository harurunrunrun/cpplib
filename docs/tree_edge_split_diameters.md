---
title: Tree Edge Split Diameters (辺切断後の木の直径)
documentation_of: ../src/algorithm/tree/tree_edge_split_diameters.hpp
---

入力順に与えた木の各辺を1本ずつ削除し、分かれた2成分の直径を求める。
直径は辺数で測り、1頂点だけの成分の直径は0とする。

## API

### `TreeEdgeSplitDiameter`

    struct TreeEdgeSplitDiameter {
        int first_diameter;
        int second_diameter;
    };

`first_diameter` は入力 `edges[i].first` を含む成分の直径、
`second_diameter` は `edges[i].second` を含む成分の直径である。
各fieldの参照は $O(1)$。

### `tree_edge_split_diameters<MAX_SIZE>(vertex_count, edges)`

返り値の `i` 番目は `edges[i]` を削除した結果に対応する。
入力辺の向きを反転すると、対応する2つの直径も入れ替わる。

## 時間計算量

`N = vertex_count` とする。

- 関数全体: $O(N)$
- 返却値の各field参照: $O(1)$

CSR隣接列の構築、木の検査、下向きDP、上向きDP、
長さ $N-1$ の返り値構築を含む。再帰は使用しない。
使用メモリは固定容量workspaceが $O(MAX\_SIZE)$、返り値が $O(N)$。

## 注意点

- `MAX_SIZE > 0` かつ `0 < vertex_count <= MAX_SIZE` が必要。
- `edges` はちょうど `vertex_count - 1` 本で、頂点番号は
  `[0, vertex_count)`、グラフは連結かつ閉路なしでなければならない。
- 自己辺、多重辺、閉路、非連結、辺数不一致、範囲外頂点、容量超過では
  `runtime_error` を送出する。
- `vertex_count == 1` では空のvectorを返す。

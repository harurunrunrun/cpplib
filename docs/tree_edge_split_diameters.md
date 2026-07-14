---
title: Tree Edge Split Diameters (辺切断後の木の直径)
documentation_of: ../src/algorithm/tree/tree_edge_split_diameters.hpp
---

入力順に与えた木の各辺を1本ずつ削除し、分かれた2成分の直径をすべて線形時間で求める。
直径とpathの長さは辺数で数え、1頂点だけの成分の直径は0とする。

## 分割直径

    struct TreeEdgeSplitDiameter {
        int first_diameter;
        int second_diameter;
    };

- first_diameter は入力 edges[i].first を含む成分の直径。
- second_diameter は入力 edges[i].second を含む成分の直径。

field参照はそれぞれ $O(1)$。

    template<int MAX_SIZE>
    vector<TreeEdgeSplitDiameter> tree_edge_split_diameters(
        int vertex_count,
        const vector<pair<int, int>>& edges
    );

返り値の i 番目は、入力 edges の i 番目の辺を削除した結果に対応する。
入力辺の向きを反転すると、対応する返り値の first_diameter と
second_diameter も反転する。

### 時間計算量

- $O(N)$

CSR隣接列の構築、木の検査、下向きDP、上向きDP、長さ $N-1$ の返り値構築を含む。
再帰は使用しない。

### 空間計算量

- 呼び出し中の固定容量workspace: $O(MAX_SIZE)$
- 返り値: $O(N)$
- それ以外の一時領域: $O(1)$

workspaceは1回のheap確保で作り、隣接列・親・走査順・DP stateを
std::arrayで保持する。

## 頂点非共有2pathの長さ積

    struct TreeTwoDisjointPathsResult {
        long long product;
        int cut_edge_index;
        int cut_u;
        int cut_v;
        int first_diameter;
        int second_diameter;
    };

- product は頂点を共有しない2本のpathの長さ積の最大値。
- cut_edge_index は最大値を与える削除辺の入力index。
- cut_u と cut_v はその辺の入力時の両端であり、
  cut_u == edges[cut_edge_index].first、
  cut_v == edges[cut_edge_index].second。
- first_diameter は cut_u を含む成分の直径。
- second_diameter は cut_v を含む成分の直径。

field参照はすべて $O(1)$。productはintの積overflowを避けるため long long。

    template<int MAX_SIZE>
    TreeTwoDisjointPathsResult
    maximum_product_of_two_vertex_disjoint_tree_paths(
        int vertex_count,
        const vector<pair<int, int>>& edges
    );

任意の頂点非共有2pathの間には、それらを別成分へ分ける辺が存在する。
逆に、任意の辺を削除した2成分の直径は頂点を共有しない。
したがって、全辺について2成分の直径積を最大化すればよい。

同じ最大積を与える辺が複数ある場合、最小の入力辺indexを返す。

### 時間計算量

- $O(N)$

分割直径を1回計算し、入力辺順に最大値を走査する。

### 空間計算量

- 固定容量workspaceと内部の分割直径列を含めて $O(MAX_SIZE + N)$
- Result自体は $O(1)$

## 注意点

- 0 < vertex_count <= MAX_SIZE。
- edges はちょうど vertex_count - 1 本で、頂点番号は
  [0, vertex_count) でなければならない。
- self-loop、多重辺、cycle、非連結、辺数不一致、範囲外頂点、
  容量超過では runtime_error を送出する。
- MAX_SIZE は正で、int indexによる隣接列長がoverflowしない値でなければならない。
- N=1では分割直径列は空。最大積Resultは product=0、
  cut_edge_index=cut_u=cut_v=-1、両直径=0。
- N>=2では積が0の場合もcut edgeを返す。
- 長さ0の1頂点pathも許すため、すべての返却直径とproductは非負。

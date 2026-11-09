---
title: Balanced Separator from Tree Decomposition (木分解からの均衡セパレータ)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/balanced_separator_from_tree_decomposition.hpp
---

木分解のバッグを 1 つ選び、そのバッグに含まれる頂点をセパレータとする。
各頂点に非負の重みが与えられたとき、そのバッグを除いた後の各成分の重みが全重みの半分以下になるバッグを返す。

## `TreeDecompositionBalancedSeparator`

```cpp
struct TreeDecompositionBalancedSeparator{
    int bag = -1;
    vector<int> vertices;
    long long maximum_component_weight = 0;
};
```

選んだバッグの番号、昇順に並べたセパレータ頂点、セパレータを除いた各成分の重みの最大値を保持する。

## `balanced_separator_from_tree_decomposition`

```cpp
TreeDecompositionBalancedSeparator
balanced_separator_from_tree_decomposition(
    const vector<vector<int>>& bags,
    const vector<pair<int, int>>& bag_edges,
    const vector<long long>& vertex_weights
);
```

`bag_edges` が作るバッグの木について、重み付き重心を求める。
頂点の重みは、その頂点が最初に現れる（番号最小の）バッグへ一度だけ割り当てる。
頂点数は `vertex_weights.size()` とする。バッグが 0 個の場合は、辺も頂点もなければデフォルト値（`bag == -1`）を返し、そうでなければ `invalid_argument` を送出する。

## API別の時間計算量・空間計算量

バッグ数を $B$、バッグの要素数の総和を $S$、頂点数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `balanced_separator_from_tree_decomposition` | $O(B + S\log S + V)$ | $O(B + S + V)$ |

## 注意点

- `bags` と `bag_edges` は有効な木分解でなければならない。この API はバッグのグラフが木である（辺数が $B-1$ で連結かつ閉路を持たない）ことを検査するが、同じ頂点を含むバッグが連結であるという条件（running intersection property）は検査しない。
- 各頂点は少なくとも 1 つのバッグに現れ、重みは非負でなければならない。同じバッグに同じ頂点が重複してはならない。
- 不正な入力には `invalid_argument` または `out_of_range`、重みの和が `long long` に収まらなければ `overflow_error` を送出する。

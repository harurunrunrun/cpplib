---
title: Balanced Separator from Tree Decomposition (木分解からの均衡セパレータ)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/balanced_separator_from_tree_decomposition.hpp
---

木分解のバッグを一つ選び、そのバッグに含まれる頂点をセパレータとする。
各頂点の非負重みについて、バッグ削除後の各成分が全重みの半分以下になるバッグを返す。

## `TreeDecompositionBalancedSeparator`

```cpp
struct TreeDecompositionBalancedSeparator{
    int bag;
    vector<int> vertices;
    long long maximum_component_weight;
};
```

選んだバッグ番号、昇順のセパレータ頂点、セパレータを除いた成分重みの最大値を保持する。

## `balanced_separator_from_tree_decomposition`

```cpp
TreeDecompositionBalancedSeparator
balanced_separator_from_tree_decomposition(
    const vector<vector<int>>& bags,
    const vector<pair<int, int>>& bag_edges,
    const vector<long long>& vertex_weights
);
```

`bag_edges` が作るバッグ木の重み付き重心を求める。
頂点重みは、その頂点が最初に現れるバッグへ一度だけ割り当てる。

## API別の時間計算量・空間計算量

バッグ数を `B`、バッグ要素数の総和を `S`、頂点数を `V` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `balanced_separator_from_tree_decomposition` | $O(B + S\log S + V)$ | $O(B + S + V)$ |

## 注意点

- `bags` と `bag_edges` は有効な木分解でなければならない。APIは連結性と閉路を検査するが、running-intersection 条件は検査しない。
- 各頂点は少なくとも一つのバッグに現れ、重みは非負でなければならない。
- 不正な入力では `invalid_argument` または `out_of_range`、重み和が収まらなければ `overflow_error` を送出する。

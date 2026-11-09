---
title: Topological Sort (トポロジカルソート) [GRL_4_B]
documentation_of: ../../../../src/algorithm/graph/traversal/topological_sort.hpp
---

有向グラフをトポロジカルソートする。

## 関数

```cpp
TopologicalSortResult topological_sort(const vector<vector<int>>& graph)
TopologicalSortResult lexicographical_topological_sort(const vector<vector<int>>& graph)
```

`TopologicalSortResult` は次を持つ。

```cpp
vector<int> order;
bool is_dag;
```

閉路がある場合、`is_dag` は `false` になる。この場合、`topological_sort` の `order` は全頂点を含むが、トポロジカル順であることは保証しない。

`topological_sort` は頂点番号の昇順に DFS を開始し、各隣接リストを格納順に走査する。返す `order` は DFS の帰りがけ順を反転した、決定的な順序になる。

`lexicographical_topological_sort` は、選択可能な頂点のうち番号最小のものを毎回選び、辞書順最小のトポロジカル順を返す。閉路がある場合、`order` には処理できた頂点だけが入り、`is_dag` は `false` になる。

## 時間計算量

- `topological_sort`: $O(N+M)$
- `lexicographical_topological_sort`: $O(N\log(N+1)+M)$

$N$ は頂点数、$M$ は辺数である。全辺の行き先の検査と、最大 $N$ 頂点の `order` の出力を含む。

## 空間計算量

- `topological_sort`: 戻り値、訪問状態、非再帰 DFS のスタックを含めて $O(N)$
- `lexicographical_topological_sort`: 戻り値、入次数の列、優先度付きキューを含めて $O(N)$

## 注意点

- `is_dag == false` のとき、`order` をトポロジカル順として使用してはならない。
- 隣接リストの行き先は $[0,N)$ に入っていなければならない。違反時は `runtime_error` を送出する。

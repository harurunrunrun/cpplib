---
title: Minimum Path Cover in a DAG (DAGの最小パス被覆)
documentation_of: ../../../../src/algorithm/matching/bipartite/minimum_path_cover_dag.hpp
---

DAGの全頂点を、互いに頂点素な最小本数の有向パスで被覆する。

## API

```cpp
MinimumPathCoverDagResult minimum_path_cover_dag(
    const vector<vector<int>>& graph
)
```

```cpp
struct MinimumPathCoverDagResult {
    int count;
    vector<vector<int>> paths;
    vector<int> successor;
    vector<int> predecessor;
};
```

- `count`: 最小パス本数
- `paths`: 各パスの頂点列。全頂点がちょうど1回現れる
- `successor[v]`: 同じパスで `v` の直後にある頂点。末尾なら `-1`
- `predecessor[v]`: 同じパスで `v` の直前にある頂点。先頭なら `-1`

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- DAG判定: $O(N+M)$
- Hopcroft--Karp法による最大二部マッチング: $O(M\sqrt N)$
- パスの復元: $O(N)$
- `minimum_path_cover_dag` 全体: $O(N+M\sqrt N)$

## 空間計算量

$O(N+M)$。

## 注意点

入力はDAGでなければならない。閉路を含む場合は `runtime_error` を送出する。隣接頂点が範囲外の場合も `runtime_error` を送出する。長さ0のパスは作らず、孤立頂点は長さ1のパスとして返す。

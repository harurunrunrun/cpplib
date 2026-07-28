---
title: Transitive Reduction (推移簡約)
documentation_of: ../../../../src/algorithm/graph/traversal/transitive_reduction.hpp
---

DAGの到達可能性を変えずに、推移的に不要な辺をすべて取り除く。

## API

```cpp
vector<pair<int, int>> transitive_reduction(
    const vector<vector<int>>& graph
)
```

簡約後の辺を辞書順で返す。同じ有向辺が複数含まれていても、戻り値には高々1本だけ含める。

## 時間計算量

頂点数を $N$、入力辺数を $M$、簡約後の辺数を $K$、$W=64$ とする。

- DAG判定とトポロジカルソート: $O(N+M)$
- 隣接頂点の整列と重複除去: $O(\sum_v d_v\log(d_v+1))$
- ビット集合による到達可能性の更新: $O(K\lceil N/W\rceil+M)$
- 戻り値の整列: $O(K\log(K+1))$

全体は $O(N+M+\sum_v d_v\log(d_v+1)+K\lceil N/W\rceil+K\log(K+1))$。

## 空間計算量

$O(N\lceil N/W\rceil+M)$。

## 注意点

入力はDAGでなければならない。閉路を含む場合は `runtime_error` を送出する。隣接頂点が範囲外の場合も `runtime_error` を送出する。

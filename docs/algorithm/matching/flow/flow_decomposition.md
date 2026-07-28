---
title: Flow Decomposition (流れ分解)
documentation_of: ../../../../src/algorithm/matching/flow/flow_decomposition.hpp
---

非負の `source`--`sink` 流を、単純な始点終点パス流と有向閉路流へ分解する。

## API

```cpp
struct FlowDecompositionEdge {
    int from;
    int to;
    long long flow;
};
```

入力有向辺と、その辺を流れる非負流量を表す。

```cpp
struct FlowDecompositionComponent {
    long long amount;
    bool is_cycle;
    vector<int> edge_ids;
};
```

- `amount`: この成分が各辺に加える正の流量
- `is_cycle == false`: `edge_ids` は `source` から `sink` への順序付きパス
- `is_cycle == true`: `edge_ids` は末尾から先頭へ戻る順序付き有向閉路

```cpp
vector<FlowDecompositionComponent> flow_decomposition(
    int vertex_count,
    const vector<FlowDecompositionEdge>& edges,
    int source,
    int sink
)
```

全成分を辺ごとに足し合わせると入力流量へ厳密に戻る。

## 時間計算量

頂点数を $N$、辺数を $M$、戻り値に格納される辺IDの総数を $L$ とする。
$L$ は最悪 $O(NM)$ である。

- 流量保存の検査: $O(N+M)$
- 各パス・閉路の抽出: 1成分当たり $O(N+M)$
- 成分数: 高々 $M$
- 全体: $O(M(N+M)+L)=O(M(N+M))$

## 空間計算量

作業領域は $O(N+M)$、戻り値を含めると $O(N+M+L)$。
各成分で正流量辺は1本以上0になるが、別成分が同じ長い経路を共有できるため $L=O(M)$ とは限らない。

## 注意点

流量は非負、頂点と `source`, `sink` は範囲内で、`source != sink` でなければならない。中間頂点の流量保存則または始点終点収支に違反すると `runtime_error` を送出する。パス流と循環流が同じ辺を共有する入力も扱える。

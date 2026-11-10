---
title: Flow Decomposition (流れ分解)
documentation_of: ../../../../src/algorithm/matching/flow/flow_decomposition.hpp
---

非負の `source`–`sink` フローを、単純な始点・終点間のパスフローと有向閉路フローに分解する。

## API

```cpp
struct FlowDecompositionEdge {
    int from;
    int to;
    long long flow;
};
```

入力の有向辺と、その辺を流れる非負の流量を表す。

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

先にパスフローをすべて取り出し、残りを閉路フローに分解する。全成分を辺ごとに足し合わせると、入力の流量に厳密に一致する。

## 時間計算量

頂点数を $N$、辺数を $M$、戻り値に格納される辺番号の総数を $L$ とする。$L$ は最悪 $O(NM)$ である。

- 流量保存の検査: $O(N+M)$
- 各パス・閉路の抽出: 1 成分あたり $O(N+M)$
- 成分数: 高々 $M$
- 全体: $O(M(N+M)+L) = O(M(N+M))$

## 空間計算量

作業領域は $O(N+M)$、戻り値を含めると $O(N+M+L)$。各成分で流量が $0$ になる辺が 1 本以上生じるが、別々の成分が同じ長い経路を共有できるため、$L = O(M)$ とは限らない。

## 注意点

流量は非負、頂点番号と `source`, `sink` は範囲内で、`source != sink` でなければならない。中間頂点での流量保存則や、始点・終点の収支に違反すると `runtime_error` を送出する。パスフローと循環フローが同じ辺を共有する入力も扱える。

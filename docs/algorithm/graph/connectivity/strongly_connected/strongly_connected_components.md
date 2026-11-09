---
title: Strongly Connected Components (強連結成分分解) [GRL_3_C] [scc]
documentation_of: ../../../../../src/algorithm/graph/connectivity/strongly_connected/strongly_connected_components.hpp
---

有向グラフの強連結成分分解を Kosaraju 法で行う。

## 関数

```cpp
StronglyConnectedComponentsResult strongly_connected_components(
    const vector<vector<int>>& graph
)
```

`StronglyConnectedComponentsResult` は次のメンバを持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
bool same_component(int first, int second) const;
```

- `id[v]`: 頂点 `v` の成分番号。成分番号は縮約した DAG のトポロジカル順になる。
- `groups[c]`: 成分 `c` の頂点列。`count == groups.size()` が成り立つ。
- `same_component(first, second)`: 2 頂点が同じ強連結成分に属するかを判定する。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- `strongly_connected_components`: $O(N + M)$
- `StronglyConnectedComponentsResult::same_component`: $O(1)$

逆グラフの構築、2 回の DFS、全頂点を `groups` へ出力する時間を含む。

## 空間計算量

- 戻り値と逆グラフを含めて $O(N + M)$
- `StronglyConnectedComponentsResult::same_component`: 追加領域 $O(1)$

## 注意点

- 各辺の行き先は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。記載した計算量には引数の検査と戻り値の構築を含む。
- `same_component` は、`id` の範囲外の頂点に対して `std::runtime_error` を送出する。
- DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。深いグラフでは `iterative_strongly_connected_components` を使う。

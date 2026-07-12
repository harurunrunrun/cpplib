---
title: Offline Dynamic Connectivity
documentation_of: ../src/structure/graph/offline_dynamic_connectivity.hpp
---

辺の追加削除が先にすべて分かっている動的連結性。

時間 `t` の状態で callback を呼び出す。

# テンプレート引数

```cpp
OfflineDynamicConnectivity<MAX_SIZE, MAX_TIME, MAX_HISTORY>
```

- 頂点数の上限 `MAX_SIZE`
- 時刻数の上限 `MAX_TIME`
- rollback DSU の履歴数の上限 `MAX_HISTORY`

# コンストラクタ

```cpp
OfflineDynamicConnectivity(int n = MAX_SIZE, int time_size = MAX_TIME)
```

`n` 頂点、時刻 `0, ..., time_size - 1` の構造を作る。

# 辺の追加削除

```cpp
void add_edge(int time, int u, int v)
void erase_edge(int time, int u, int v)
```

`add_edge` は時刻 `time` から辺を有効にする。

`erase_edge` は時刻 `time` から辺を無効にする。同じ辺が複数回追加された場合は 1 本だけ無効にする。

## 時間計算量

- $O(\log Q)$

# 実行

```cpp
dc.run([&](int time, const auto& dsu){
    // dsu.same(u, v)
});
```

各時刻の状態で callback を呼び出す。callback には rollback DSU が渡される。

## 時間計算量

- $O((Q + M \log Q) \log N)$

`M` は辺が有効な区間数。

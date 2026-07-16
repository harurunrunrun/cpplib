---
title: Offline Dynamic Connectivity (オフライン動的連結性)
documentation_of: ../src/structure/graph/offline_dynamic_connectivity.hpp
---

辺の追加削除が先にすべて分かっている動的連結性。

時間 `t` の状態で callback を呼び出す。

## テンプレート引数

```cpp
OfflineDynamicConnectivity<MAX_SIZE, MAX_TIME, MAX_HISTORY>
```

- 頂点数の上限 `MAX_SIZE`
- 時刻数の上限 `MAX_TIME`
- rollback DSU の履歴数の上限 `MAX_HISTORY`

## コンストラクタ

```cpp
OfflineDynamicConnectivity(int n = MAX_SIZE, int time_size = MAX_TIME)
```

`n` 頂点、時刻 `0, ..., time_size - 1` の構造を作る。

## 時間計算量

- $O(\mathtt{MAX\_TIME})$

区間segment treeの各nodeに対応する空の辺列を構築する。

## 辺の追加削除

```cpp
void add_edge(int time, int u, int v)
void erase_edge(int time, int u, int v)
```

`add_edge` は時刻 `time` から辺を有効にする。

`erase_edge` は時刻 `time` から辺を無効にする。同じ辺が複数回追加された場合は 1 本だけ無効にする。

## 時間計算量

- `add_edge`: 辺種数を $E$ として amortized $O(\log E)$
- `erase_edge`: $O(\log E+\log Q)$

## 実行

```cpp
dc.run([&](int time, const auto& dsu){
    // dsu.same(u, v)
});
```

各時刻の状態で callback を呼び出す。callback には rollback DSU が渡される。

## 時間計算量

- $O((Q + M \log Q) \log N+\sum_{t=0}^{Q-1} C_t)$

`M` は辺が有効な区間数、$C_t$ は時刻 $t$ のcallback実行時間。初回`run`のbuildは $O(M\log Q)$ で、この式に含まれる。

## 状態情報

`size`, `time_size` は $O(1)$。

## 空間計算量（API別の追加領域）

- constructor、`size`、`time_size`: $O(1)$（segment-tree container自体は本体へ保存）
- `add_edge`、`erase_edge`: 有効辺instanceごとに本体へ $O(1)$、一時領域は $O(1)$
- 初回`run`: 有効区間をsegment treeへ展開する保存領域 $O(M\log Q)$、DFS stack $O(\log Q)$、rollback履歴は同時に有効な辺数に比例
- 2回目以降の`run`: 追加の辺保存領域は $O(1)$、DFS stackとrollback履歴は初回と同じ

run 中はこのほかheap上に rollback DSU の固定領域 O(MAX_SIZE + MAX_HISTORY) を保持する。
callback自身が使う領域は含めない。

## 注意点

- `0 <= n <= MAX_SIZE`, `0 <= time_size <= MAX_TIME`。違反時は例外。
- event時刻は`[0,time_size)`、頂点は`[0,n)`。違反時は例外。
- 存在しない辺の`erase_edge`、および初回`run`後のevent追加・削除は例外。
- 未削除の辺は`time_size`まで有効として初回`run`で確定する。`run`自体は再実行できる。
- DFS中の同時merge履歴数が`MAX_HISTORY`を超えるとrollback DSU由来の例外。

---
title: Offline Dynamic Connectivity (オフライン動的連結性)
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/offline_dynamic_connectivity.hpp
---

辺の追加・削除がすべて先に分かっている場合の動的連結性。
辺の有効区間を時刻上のセグメント木に載せ、ロールバック可能な DSU で DFS しながら、各時刻 $t$ の状態でコールバックを呼び出す。

## テンプレート引数

```cpp
OfflineDynamicConnectivity<MAX_SIZE = -1, MAX_TIME = -1, MAX_HISTORY = -1>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_TIME`: 時刻数の上限
- `MAX_HISTORY`: 内部のロールバック DSU（`RollbackDSU`）の履歴数の上限

省略（`-1`）した上限は、時刻のセグメント木とロールバック DSU の配列・履歴に `std::vector` を使う可変長になる。コピーはできず、ムーブはできる。

```cpp
OfflineDynamicConnectivity<> dynamic(vertex_count, time_count);
OfflineDynamicConnectivity<100000, 200000, 200000> fixed(vertex_count, time_count);
```

## コンストラクタ

```cpp
explicit OfflineDynamicConnectivity(int n = MAX_SIZE, int time_size = MAX_TIME)
```

$n$ 頂点、時刻 $0, \dots, \texttt{time_size} - 1$ の構造を作る。区間セグメント木の各ノードに対応する空の辺列を構築する。

## 辺の追加・削除

```cpp
void add_edge(int time, int u, int v)
void erase_edge(int time, int u, int v)
```

`add_edge` は時刻 `time` から辺を有効にする。

`erase_edge` は時刻 `time` から辺を無効にする。同じ辺が複数回追加されている場合は、最後に追加した 1 本だけを無効にする。

## 実行

```cpp
dc.run([&](int time, const auto& dsu){
    // dsu.same(u, v)
});
```

各時刻の状態でコールバックを呼び出す。コールバックの第 2 引数には、その時刻の連結性を表す `RollbackDSU` が渡される。

## size / time_size

```cpp
int size() const
int time_size() const
```

頂点数と時刻数を返す。

## 時間計算量

頂点数を $N$、時刻数を $Q$、辺が有効な区間の数を $M$、異なる辺の種類数を $E$、時刻 $t$ のコールバックの実行時間を $C_t$ とする。

- コンストラクタ: $O(Q)$（固定長版は $O(\texttt{MAX_TIME})$）
- `add_edge`: 償却 $O(\log E)$
- `erase_edge`: $O(\log E+\log Q)$
- `run`: $O((Q + M \log Q) \log N+\sum_{t=0}^{Q-1} C_t)$。初回の `run` で行う、有効区間のセグメント木への展開 $O(M\log Q)$ もこれに含まれる。ロールバック DSU の構築に、さらに $O(N)$（固定長版は $O(\texttt{MAX_SIZE}+\texttt{MAX_HISTORY})$）かかる
- `size`、`time_size`: $O(1)$

## 空間計算量

全体で $O(N+Q+M\log Q)$。

- コンストラクタ、`size`、`time_size`: 追加領域 $O(1)$（セグメント木の容器自体は本体に保存する）
- `add_edge`、`erase_edge`: 有効な辺 1 本ごとに本体へ $O(1)$。一時領域は $O(1)$
- 初回の `run`: 有効区間をセグメント木に展開する保存領域 $O(M\log Q)$、DFS のスタック $O(\log Q)$、ロールバックの履歴は根からのパス上の辺数に比例する
- 2 回目以降の `run`: 辺の保存領域は増えず、DFS のスタックとロールバックの履歴は初回と同じ
- `run` の実行中は、このほかにロールバック DSU の領域 $O(N + H)$（固定長版は $O(\texttt{MAX_SIZE} + \texttt{MAX_HISTORY})$）をヒープに確保する

コールバック自身が使う領域は含めない。

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$、$0 \le \texttt{time_size} \le \texttt{MAX_TIME}$ でなければならない。違反すると `std::runtime_error` を送出する。
- イベントの時刻は $[0, \texttt{time_size})$、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- 存在しない辺の `erase_edge`、および初回の `run` の後のイベントの追加・削除は `std::runtime_error` を送出する。
- 削除されていない辺は `time_size` まで有効として、初回の `run` で確定する。`run` 自体は何度でも実行できる。
- 自己ループは連結性に影響しないので無視する。
- DFS 中に同時に保持する `merge` の履歴数が `MAX_HISTORY` を超えると、ロールバック DSU が例外を送出する。

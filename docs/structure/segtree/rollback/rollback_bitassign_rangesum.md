---
title: Rollback BitAssign RangeSum (ロールバック可能ビット代入・区間和)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_bitassign_rangesum.hpp
---

ロールバック可能なビット列に対する区間代入・区間和。変更前のノードを履歴に保存し、指定した状態より後の更新を破棄できる。

```cpp
RollbackBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

同じ API の別名 `RollbackDynamicBitAssignRangeSum` も定義される。

## API

```cpp
explicit RollbackBitAssignRangeSum(long long n = MAX_SIZE)  // 可変長版の既定値は 0
int assign(long long l, long long r, bool value)
int flip(long long l, long long r)
int set_one(long long l, long long r)
int set(long long l, long long r)
long long sum(long long l, long long r) const
bool get(long long k) const
int fork()
int snapshot() const
void rollback(int snapshot)
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

- コンストラクタ: 長さ $n$ の全 0 列を状態 0 として作る。
- `assign(l, r, value)`, `flip(l, r)`: 現在の状態の半開区間 $[l, r)$ を `value` に代入する・反転する。
- `set_one(l, r)`, `set(l, r)`: 現在の状態の半開区間 $[l, r)$ を 1 に上書きする。
- 各更新と `fork()` は新しい状態番号を返す。`fork()` は内容を変えずに状態番号だけを進める。
- `sum(l, r)`, `get(k)`: 現在の状態の 1 の個数・1 点のビットを返す。
- `snapshot()`: 現在の状態番号（`latest_version()` と同じ）を返す。
- `rollback(s)`: 状態 `s` より後の更新をすべて破棄し、状態 `s` に戻す。
- `size()`, `versions()`, `latest_version()`, `nodes_used()`, `changes_used()`: 列の長さ・状態数・最新の状態番号・使用ノード数・変更記録数を返す。

## 時間計算量

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS}+\texttt{MAX_CHANGES})$
- `assign`, `flip`, `set_one`, `set`, `sum`, `get`: $O(\log \texttt{MAX_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

更新 1 回で $O(\log \texttt{MAX_SIZE})$ 個のノードと変更記録を使い、クエリは $O(\log \texttt{MAX_SIZE})$ の再帰スタックを使う。

## 注意点

- `rollback` で破棄された状態番号は無効になる。
- `0 < n <= MAX_SIZE` で、半開区間と点が有効でなければならない。
- `MAX_VERSIONS` は状態 0 を含む状態数の上限である。
- 範囲・状態番号の違反、ノード・状態数・変更記録の容量超過では `std::runtime_error` を送出する。失敗した更新は本体と履歴を元に戻す。
- テンプレート引数をすべて省略すると可変長版になり、`RollbackBitAssignRangeSum<> tree(n);` のように使う。ノード・状態・履歴は使用量に応じて `std::vector` を拡張する。

---
title: Persistent BitOverwrite RangeSum (完全永続ビット上書き・区間和)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_bitoverwrite_rangesum.hpp
---

完全永続なビット列に対する区間 1 上書き・区間和。経路複製（path copy）を行う疎な暗黙セグメント木で実装している。

```cpp
PersistentBitOverwriteRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

同じ API の別名 `PersistentDynamicBitOverwriteRangeSum` も定義される。

型名上の主な操作は 1 への上書き（`set`）だが、基底クラスの `assign`, `flip`, `set_one` も利用できる。

## API

```cpp
explicit PersistentBitOverwriteRangeSum(long long n = MAX_SIZE)  // 可変長版の既定値は 0
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int fork(int version)
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
```

- コンストラクタ: 長さ $n$ の全 0 列を version 0 として作る。
- `size()`, `versions()`, `latest_version()`, `nodes_used()`: 列の長さ・version 数・最新 version 番号・使用中の疎セグメント木のノード数を返す。
- `fork(version)`: 指定 version と同じ内容の新しい version を返す。
- `assign(version, l, r, value)`: 指定 version の半開区間 $[l, r)$ を `value`（0 または 1）に代入した新しい version を返す。
- `flip(version, l, r)`: 指定 version の半開区間 $[l, r)$ を反転した新しい version を返す。
- `set_one(version, l, r)`: 指定 version の半開区間 $[l, r)$ を 1 に代入した新しい version を返す。
- `set(version, l, r)`: `set_one` と同じ。
- `sum(version, l, r)`: 指定 version の半開区間 $[l, r)$ にある 1 の個数を返す。
- `get(version, k)`: 指定 version の $k$ 番目のビットを返す。

## 時間計算量

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `assign`, `flip`, `set_one`, `set`, `sum`, `get`: $O(\log \texttt{MAX_SIZE})$
- `fork` と状態取得関数: $O(1)$

更新 1 回で $O(\log \texttt{MAX_SIZE})$ 個のノードを追加し、クエリは $O(\log \texttt{MAX_SIZE})$ の再帰スタックを使う。

## 注意点

- 任意の有効な version から分岐でき、既存の version は変更されない。
- `0 < n <= MAX_SIZE`、`0 <= l <= r <= n`、`0 <= k < n` が必要である。
- `MAX_VERSIONS` は version 0 を含む総 version 数の上限である。空区間の更新も version を 1 つ消費する。
- version・範囲の違反、ノード・version の容量超過では `std::runtime_error` を送出する。失敗した更新は version 数とノード使用量を元に戻す。
- テンプレート引数をすべて省略すると可変長版になり、`PersistentBitOverwriteRangeSum<> tree(n);` のように使う。ノードと version は使用量に応じて `std::vector` を拡張する。

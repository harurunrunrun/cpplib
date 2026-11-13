---
title: Offline Persistent Disjoint Set Union (オフライン完全永続素集合データ構造)
documentation_of: ../../../../src/structure/dsu/persistent/offline_persistent_dsu.hpp
---

バージョン木を先に構築し、ロールバック付きの 1 回の走査で各バージョンの連結性クエリをまとめて処理する、オフラインの完全永続 Union-Find。

## テンプレート引数

```cpp
OfflinePersistentDSU<MAX_SIZE = -1, MAX_VERSION = -1, MAX_QUERY = MAX_VERSION>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_VERSION`: 初期バージョンを除くバージョン数の上限
- `MAX_QUERY`: 登録するクエリ数の上限（デフォルトは `MAX_VERSION`）

省略（`-1`）した上限は、実行時の要素数に合わせて `std::vector` を確保する可変長になる。
`MAX_VERSION` $\le$ `INT_MAX / 2`、`MAX_QUERY` $\lt$ `INT_MAX` が必要である。
コピーはできず、ムーブはできる。

```cpp
using Dynamic = OfflinePersistentDSU<>;
```

## コンストラクタ

```cpp
explicit OfflinePersistentDSU(int size = MAX_SIZE)
```

互いに素な `size` 頂点を持つバージョン $0$ を作る。可変長版では `size` のデフォルトは $0$ である。

## merge / fork

```cpp
int merge(int version, int left, int right)
int fork(int version)
```

`merge` は `version` で 2 頂点を併合した新しいバージョンを、`fork` は `version` と内容が同じ新しいバージョンを追加し、その番号を返す。実際の併合は `solve` まで遅延する。

## query_same

```cpp
int query_same(int version, int left, int right)
```

`version` で 2 頂点が同じ成分に属するかを問うクエリを登録し、$0$ 始まりのクエリ番号を返す。

## solve

```cpp
vector<int> solve()
```

登録順に `query_same` の答えを `0` または `1` で返す。同じ内容に対して何度でも呼び出せる。

## size / versions / query_count

```cpp
int size() const
int versions() const
int query_count() const
```

頂点数、初期バージョンを含むバージョン数、登録済みのクエリ数を返す。

## 時間計算量

作成したバージョン数を $V$、クエリ数を $Q$、頂点数を $N$ とする。

- コンストラクタ: $O(N)$。固定長版ではバージョンごとの配列の確保に $O(\texttt{MAX_VERSION})$ かかる
- `merge`、`fork`、`query_same`: 償却 $O(1)$
- `solve`: $O((V+Q)\log(N+1))$
- `size`、`versions`、`query_count`: $O(1)$

## 空間計算量

- 本体: $O(N+V+Q)$（固定長版では $O(N+\texttt{MAX_VERSION})$ にクエリ数分が加わる）
- `solve` の返り値と走査スタック: $O(V+Q)$

## 注意点

- 頂点やバージョンが範囲外の場合は `std::out_of_range`、各固定容量を超えた場合は `std::length_error` を送出する。
- union by size を使い、経路圧縮は行わない。

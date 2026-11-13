---
title: Persistent Disjoint Set Union (完全永続素集合データ構造)
documentation_of: ../../../../src/structure/dsu/persistent/persistent_dsu.hpp
---

完全永続な素集合データ構造。各バージョンの `parent_or_size` を永続配列（永続セグメント木）で持ち、`merge` は指定したバージョンから新しいバージョンを作る。
union by size で、経路圧縮は行わない。

## テンプレート引数

```cpp
PersistentDSU<MAX_SIZE = -1, MAX_VERSION = -1>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_VERSION`: 追加できるバージョン数の上限（初期バージョンを除く）

省略（`-1`）した上限は、実行時の要素数に合わせて `std::vector` を確保する可変長になる。固定長版では、ノード領域として $2 \cdot \texttt{MAX_VERSION} \cdot (\lceil \log_2 \texttt{MAX_SIZE} \rceil + 1)$ 個を確保する。

```cpp
using Dynamic = PersistentDSU<>;
```

## コンストラクタ

```cpp
explicit PersistentDSU(int n = MAX_SIZE)
```

$n$ 頂点の初期バージョン $0$ を作る。可変長版では `n` のデフォルトは $0$ である。

## merge / fork

```cpp
int merge(int version, int u, int v)
int fork(int version)
```

`merge` は `version` で $u$ と $v$ を併合した新しいバージョンを作り、その番号を返す。既に同じ集合の場合も新しいバージョンを作る。
`fork` は `version` と同じ状態を指す新しいバージョンを作る。

## same / leader / component_size / groups

```cpp
bool same(int version, int u, int v) const
int leader(int version, int v) const
int component_size(int version, int v) const
int groups(int version) const
```

指定したバージョンでの連結性・代表元・成分の頂点数・成分数を返す。

## size / versions / latest_version / nodes_used

```cpp
int size() const
int versions() const
int latest_version() const
int nodes_used() const
```

頂点数、作られたバージョン数、最後に作られたバージョンの番号、使用中の永続配列のノード数を返す。

## 時間計算量

$N$ を頂点数、$S$ を永続配列の幅（固定長版では $\texttt{MAX_SIZE}$、可変長版では $N$）とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `merge`: $O(\log N \log S)$
- `same`、`leader`、`component_size`: $O(\log N \log S)$
- `fork`、`groups`、`size`、`versions`、`latest_version`、`nodes_used`: $O(1)$

## 空間計算量

- 異なる成分の `merge` は永続配列のノードを $O(\log S)$ 個生成する。同じ成分の `merge` と `fork` はノードを生成しない
- 本体: 可変長版は $O(N + V + M \log S)$（$V$ はバージョン数、$M$ は異なる成分の `merge` の回数）
- `set` の再帰スタックは $O(\log S)$、それ以外の関数の追加領域は $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$、バージョンは $[0, \mathrm{versions}())$ でなければならない。違反すると `std::runtime_error` を送出する。
- `fork` と `merge` はバージョンを 1 個追加する。追加したバージョン数が `MAX_VERSION` を超えると `std::runtime_error` を送出する。
- ノード容量を超えた場合は、ノード数とバージョン数を操作前の状態に戻してから例外を送出する。

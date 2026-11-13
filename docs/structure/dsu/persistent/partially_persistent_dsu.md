---
title: Partially Persistent DSU (部分永続素集合データ構造)
documentation_of: ../../../../src/structure/dsu/persistent/partially_persistent_dsu.hpp
---

過去の状態を参照できる部分永続な素集合データ構造。更新は最新の状態にだけ行える。
union by size で、経路圧縮は行わない。

## テンプレート引数

```cpp
PartiallyPersistentDSU<MAX_SIZE = -1, MAX_UPDATE = -1>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_UPDATE`: 更新（`merge`）回数の上限

省略（`-1`）した上限は、実行時の要素数・更新回数に合わせて `std::vector` を確保する可変長になる。

```cpp
using Dynamic = PartiallyPersistentDSU<>;
```

## コンストラクタ

```cpp
explicit PartiallyPersistentDSU(int n = MAX_SIZE)
```

バージョン $0$ として $n$ 頂点の集合を作る。可変長版では `n` のデフォルトは $0$ である。

## merge

```cpp
int merge(int u, int v)
```

最新のバージョンで $u$ と $v$ を併合し、新しいバージョンの番号を返す。既に同じ集合でも新しいバージョンを作る。

## same / leader / component_size / groups

```cpp
bool same(int version, int u, int v) const
int leader(int version, int v) const
int component_size(int version, int v) const
int groups(int version) const
```

指定したバージョンでの情報を返す。`leader` は代表元、`component_size` は成分の頂点数、`groups` は成分数である。

## size / versions / latest_version

```cpp
int size() const
int versions() const
int latest_version() const
```

頂点数、現在存在するバージョン数、最新のバージョン番号を返す。

## 時間計算量

$N$ を頂点数、$U$ を更新回数とする。

- コンストラクタ: $O(N)$。固定長版では配列の初期化に $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$ かかる
- `merge`: $O(\log N)$
- `leader`、`same`: $O(\log N)$
- `component_size`: $O(\log N + \log U)$
- `groups`、`size`、`versions`、`latest_version`: $O(1)$

## 空間計算量

- 本体: $O(N+U)$（固定長版では $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$）
- `merge` は $O(1)$ 個の履歴を本体に追加する。各関数の呼び出し中の一時領域は $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$、バージョンは $[0, \mathrm{versions}())$ でなければならない。違反すると `std::runtime_error` を送出する。
- バージョン $0$ が初期状態である。
- 更新は高々 `MAX_UPDATE` 回。超過した `merge` は `std::runtime_error` を送出し、既存のバージョンを変えない。

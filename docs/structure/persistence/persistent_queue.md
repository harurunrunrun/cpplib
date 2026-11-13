---
title: Persistent Queue (完全永続キュー)
documentation_of: ../../../src/structure/persistence/persistent_queue.hpp
---

任意の過去のバージョンを保ったまま、末尾への追加または先頭の削除を行った新しいバージョンを作るキューである。バージョン 0 は空のキューであり、更新に成功するたびに連番のバージョンを 1 つ生成する。

## テンプレート引数

```cpp
template<class PersistentStorage, int MAX_VERSION = -1>
struct PersistentQueue

template<class T, int MAX_SIZE = -1, int MAX_VERSION = -1>
using PersistentArrayQueue = PersistentQueue<PersistentArray<T, MAX_SIZE, MAX_VERSION>, MAX_VERSION>;

template<auto Monoid, int MAX_SIZE = -1, int MAX_VERSION = -1>
using PersistentSegtreeQueue = PersistentQueue<PersistentSegtree<Monoid, MAX_SIZE, MAX_VERSION>, MAX_VERSION>;
```

`PersistentStorage` は、バージョン付きの `get(version, index)` と、新しいバージョン番号を返す `set(version, index, value)`、および `size()` を持つ永続ランダムアクセスストレージとする。`value_type` は `get` の戻り値の型から決まる。

`PersistentArrayQueue` は `PersistentArray` を、`PersistentSegtreeQueue` は `PersistentSegtree` をストレージに使う別名である。後者では `Monoid::S` がキューの値型になる。キューは区間積を使わないが、`Monoid` は通常どおり結合則と単位元を満たす必要がある。

`MAX_SIZE` は同時に保持できる要素数の上限、`MAX_VERSION` は `push` と `pop` で生成できるバージョン数の合計の上限である。省略時（`-1`）は上限を設けず、実行時の要素数・更新数に合わせて `std::vector` で領域を確保する。

## 構築

```cpp
explicit PersistentQueue(int capacity)
```

要素数の上限を `capacity` として空のキューを構築する。初期バージョンは 0 である。別名を使い `MAX_SIZE` を指定する場合は `0 <= capacity <= MAX_SIZE` を要求する。

時間計算量:

- `PersistentArrayQueue`: $O(\texttt{MAX_SIZE} + \texttt{MAX_VERSION})$
- `PersistentSegtreeQueue`: $O(\texttt{MAX_SIZE} + \texttt{MAX_VERSION}\log \texttt{MAX_SIZE})$

容量を省略した場合は `capacity` に比例する。

## バージョンと大きさ

```cpp
int capacity() const
int versions() const
int latest_version() const
int size(int version) const
bool empty(int version) const
```

`capacity` は要素数の上限、`versions` は現在存在するバージョン数、`latest_version` は最後に生成したバージョン番号を返す。`size` と `empty` は指定したバージョンの状態を調べる。

時間計算量はすべて $O(1)$。

## 先頭要素

```cpp
value_type front(int version) const
```

指定したバージョンの先頭要素を値で返す。空のキューでは `std::runtime_error` を送出する。

時間計算量は `PersistentStorage::get` の時間計算量であり、2 つの別名では $O(\log \texttt{MAX_SIZE})$（容量省略時は $O(\log(n+1))$、$n$ は `capacity`）である。

## 末尾への追加

```cpp
int push(int version, const value_type& value)
```

指定したバージョンの末尾へ `value` を追加した新しいバージョンを作り、その番号を返す。指定したバージョンは変更しない。要素数が `capacity` に達している場合は `std::runtime_error` を送出する。

時間計算量は `PersistentStorage::set` の時間計算量であり、2 つの別名では時間・新しく使用するノード数ともに $O(\log \texttt{MAX_SIZE})$ である。

## 先頭の削除

```cpp
int pop(int version)
```

指定したバージョンの先頭を削除した新しいバージョンを作り、その番号を返す。指定したバージョンは変更しない。空のキューでは `std::runtime_error` を送出する。

時間計算量は $O(1)$。ストレージの新しいバージョンは作らず、キューのバージョン管理情報だけを追加する。

## 空間計算量

2 つの別名は、バージョン管理情報に $O(\texttt{MAX_VERSION})$ を予約する。ストレージを含む予約領域全体は $O(\texttt{MAX_SIZE} + \texttt{MAX_VERSION}\log \texttt{MAX_SIZE})$ である。`push` が実際に使用状態へ移すノード数は 1 バージョンあたり $O(\log \texttt{MAX_SIZE})$ である。容量を省略した場合、要素数 $n$、`push` の回数 $u$、バージョン数 $v$ に対して $O(n + u\log(n+1) + v)$ となる。

## 注意点

- バージョンは 0 から始まり、成功した `push` または `pop` が返す番号は常に直前の `latest_version() + 1` である。`front`、`size`、`empty` はバージョンを生成しない。
- 範囲外のバージョン、空のキューへの `front` または `pop`、容量超過、バージョン数の上限超過では `std::runtime_error` を送出する。
- 内部の添字は循環させるので、`pop` 後に別の要素を `push` しても、同時に保持する要素数が `capacity` 以下なら領域を再利用できる。過去のバージョンの値はストレージの永続性により変化しない。
- コピー・ムーブはできない（削除されている）。

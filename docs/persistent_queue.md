---
title: Persistent Queue (完全永続キュー)
documentation_of: ../src/structure/other/persistent_queue.hpp
---

任意の過去versionを保ったまま、末尾への追加または先頭の削除を行った新しいversionを作るqueue。
version 0は空queueであり、更新に成功するたびに連番のversionを1つ生成する。

## テンプレート引数

```cpp
template<class PersistentStorage, int MAX_VERSION>
struct PersistentQueue

template<class T, int MAX_SIZE, int MAX_VERSION>
using PersistentArrayQueue = ...

template<auto Monoid, int MAX_SIZE, int MAX_VERSION>
using PersistentSegtreeQueue = ...
```

`PersistentStorage`は、version付きの`get(version, index)`と`set(version, index, value)`、
および`size()`を持つ永続random-access storageとする。`value_type`は`get`の返り値から決まる。

`PersistentArrayQueue`は`PersistentArray`、`PersistentSegtreeQueue`は
`PersistentSegtree`をstorageに使うaliasである。後者の`Monoid::S`がqueueの値型になる。
queueはsegment積を使わないが、`Monoid`は通常どおり結合則と単位元を満たす必要がある。

`MAX_SIZE`は同時に保持できる要素数、`MAX_VERSION`は`push`と`pop`で生成できる
version数の合計上限である。

## 構築

```cpp
Queue queue(int capacity)
```

要素数上限を`capacity`として空queueを構築する。初期versionは0。
aliasを使う場合は`0 <= capacity <= MAX_SIZE`を要求する。

## 時間計算量

- `PersistentArrayQueue`: $O(MAX\_SIZE + MAX\_VERSION)$
- `PersistentSegtreeQueue`: $O(MAX\_SIZE + MAX\_VERSION\log MAX\_SIZE)$

## versionと大きさ

```cpp
int capacity() const
int versions() const
int latest_version() const
int size(int version) const
bool empty(int version) const
```

`capacity`は要素数上限、`versions`は現在存在するversion数、`latest_version`は
最後に生成したversion番号を返す。`size`と`empty`は指定versionの状態を調べる。

## 時間計算量

すべて$O(1)$。

## 先頭要素

```cpp
value_type front(int version) const
```

指定versionの先頭要素を値で返す。空queueでは`runtime_error`。

## 時間計算量

`PersistentStorage::get`の時間計算量。2つのaliasでは
$O(\log MAX\_SIZE)$。

## 末尾への追加

```cpp
int push(int version, const value_type& value)
```

指定versionの末尾へ`value`を追加した新versionを作り、その番号を返す。
指定versionは変更しない。要素数が`capacity`に達している場合は`runtime_error`。

## 時間計算量

`PersistentStorage::set`の時間計算量。2つのaliasでは次のとおり。

- 時間・新しく使用するnode: $O(\log MAX\_SIZE)$

## 先頭の削除

```cpp
int pop(int version)
```

指定versionの先頭を削除した新versionを作り、その番号を返す。
指定versionは変更しない。空queueでは`runtime_error`。

## 時間計算量

$O(1)$。storageの新versionは作らず、queueのversion metadataだけを追加する。

## 空間計算量

2つのaliasはmetadataに$O(MAX\_VERSION)$を予約する。storageを含む予約領域全体は
$O(MAX\_SIZE + MAX\_VERSION\log MAX\_SIZE)$である。
`push`が実際に使用状態へ移すnode数は1versionあたり$O(\log MAX\_SIZE)$。

## 注意点

versionは0から始まり、成功した`push`または`pop`が返す番号は常に
直前の`latest_version() + 1`である。`front`、`size`、`empty`はversionを生成しない。
範囲外version、空queueへの`front`または`pop`、容量超過、version上限超過では
`runtime_error`を送出する。

内部添字は循環させるため、`pop`後に別の要素を`push`しても、同時要素数が
`capacity`以下なら領域を再利用できる。過去versionの値はstorageの永続性により変化しない。

---
title: Persistent Queue Queries (クエリ履歴付き完全永続キュー) [persistent_queue]
documentation_of: ../src/structure/other/persistent_queue_queries.hpp
---

各操作の番号をversion識別子として扱う完全永続queue wrapper。
先頭の取得と削除を1回の `pop` にまとめ、呼び出し側で内部version番号を管理しない。

## テンプレート引数

```cpp
PersistentQueueQueries<Queue, MAX_QUERY>
```

`Queue` は `value_type`、`push(version,value)`、`front(version)`、
`pop(version)` を持ち、更新ごとに連番のversionを作る完全永続queueとする。
`MAX_QUERY` は登録できる操作数の上限。backendはheapに確保される。

## コンストラクタ

```cpp
explicit PersistentQueueQueries(int capacity)
```

同時に保持できる要素数を `capacity` として空queueを作る。初期状態の操作番号は `-1`。

## 時間計算量

使用する `Queue` の構築時間。

## `processed_queries`

```cpp
int processed_queries() const
```

登録済み操作数を返す。

## 時間計算量

$O(1)$。

## `push`

```cpp
void push(int base_query, const value_type& value)
```

操作 `base_query` の直後のqueueの末尾へ `value` を追加し、現在の操作番号へ新状態を登録する。

## 時間計算量

`Queue::push` と同じ。`PersistentArrayQueue` と `PersistentSegtreeQueue` では
$O(\log MAX\_SIZE)$。

## `pop`

```cpp
value_type pop(int base_query)
```

操作 `base_query` の直後のqueueの先頭値を返し、その要素を削除した状態を現在の操作番号へ登録する。

## 時間計算量

`Queue::front` と `Queue::pop` の和。上記2つのqueue aliasでは
$O(\log MAX\_SIZE)$。

## 空間計算量

wrapper本体は $O(1)$。backendの固定長予約領域はheapに置かれ、その空間量は `Queue` に従う。

## 注意点

- `base_query == -1` は初期状態を表す。それ以外は `[0, processed_queries())` が必要。
- 空queueへの `pop`、要素容量・操作容量・backend version容量の超過、範囲違反では `runtime_error`。
- 失敗した操作は操作数を増やさない。
- 各成功操作はbackendにちょうど1つ新versionを作る `Queue` を要求する。
- copyは禁止し、moveを許可する。

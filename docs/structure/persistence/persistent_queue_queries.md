---
title: Persistent Queue Queries (クエリ履歴付き完全永続キュー) [persistent_queue]
documentation_of: ../../../src/structure/persistence/persistent_queue_queries.hpp
---

各操作の番号をバージョンの識別子として扱う、完全永続キューのラッパーである。先頭の取得と削除を 1 回の `pop` にまとめ、呼び出し側で内部のバージョン番号を管理しなくてよい。

## テンプレート引数

```cpp
template<class Queue, int MAX_QUERY = -1>
class PersistentQueueQueries;
```

`Queue` は `value_type`、`push(version, value)`、`front(version)`、`pop(version)` を持ち、更新ごとに連番のバージョンを作る完全永続キュー（`PersistentArrayQueue` など）とする。`MAX_QUERY` は登録できる操作数の上限で、省略時（`-1`）は上限を設けない。内部のキューはヒープに確保される。

## コンストラクタ

```cpp
explicit PersistentQueueQueries(int capacity)
```

同時に保持できる要素数を `capacity` として空のキューを作る。初期状態の操作番号は `-1` である。

時間計算量は、使用する `Queue` の構築時間である。

## `processed_queries`

```cpp
int processed_queries() const
```

登録済みの操作数を返す。時間計算量は $O(1)$。

## `push`

```cpp
void push(int base_query, const value_type& value)
```

操作 `base_query` の直後のキューの末尾へ `value` を追加し、その状態を現在の操作番号（`processed_queries()` の呼び出し前の値）に登録する。

時間計算量は `Queue::push` と同じで、`PersistentArrayQueue` と `PersistentSegtreeQueue` では $O(\log \texttt{MAX_SIZE})$ である。

## `pop`

```cpp
value_type pop(int base_query)
```

操作 `base_query` の直後のキューの先頭の値を返し、その要素を削除した状態を現在の操作番号に登録する。

時間計算量は `Queue::front` と `Queue::pop` の和で、上記 2 つの別名では $O(\log \texttt{MAX_SIZE})$ である。

## 空間計算量

ラッパー本体は $O(1)$。内部キューの予約領域はヒープに置かれ、その大きさは `Queue` に従う。容量を省略した場合、操作数を $q$ として管理情報の保存領域は $O(q)$ である。

## 注意点

- `base_query == -1` は初期状態を表す。それ以外は `[0, processed_queries())` の範囲にある必要がある。
- 空のキューへの `pop`、要素数の上限・操作数の上限・内部キューのバージョン数の上限の超過、範囲違反では `std::runtime_error` を送出する。
- 失敗した操作は操作数を増やさない。
- 成功した各操作が内部キューにちょうど 1 つ新しいバージョンを作るような `Queue` を要求する。
- コピーはできず、ムーブはできる。

---
title: Partially Retroactive Priority Queue (部分遡及優先度付きキュー)
documentation_of: ../src/structure/heap/partially_retroactive_priority_queue.hpp
---

priority queueへのpush・pop操作を時刻順に保持し、過去時刻への操作追加と操作削除を反映した現在状態を扱う。query対象が現在状態だけなので部分retroactiveである。

```cpp
template<
    class Time,
    class T,
    size_t MAX_OPERATION,
    class Compare = less<T>,
    class TimeCompare = less<Time>
>
class PartiallyRetroactivePriorityQueue;
```

`Compare` で最も小さい要素をpop・`top`の対象とする。defaultの `less<T>` ではminimum priority queueになる。時刻の同一性は `TimeCompare` による同値で判定する。

内部では、現在まで残るpushをactive、pop済みのpushをinactiveとして管理する。時刻treapにbridge用prefix集約、queueサイズのprefix minimum、active値のminimum、inactive値のmaximumを保持するため、連鎖的にpop対象が変わる場合も履歴を再実行しない。

## Operation kind

```cpp
enum class RetroactivePriorityQueueOperationKind {
    push,
    pop
};
```

## Constructor

```cpp
PartiallyRetroactivePriorityQueue(
    Compare compare = Compare{},
    TimeCompare time_compare = TimeCompare{}
);
```

空の履歴と空の現在queueを作る。

## `capacity`, `operation_count`, `size`, `empty`

```cpp
static constexpr size_t capacity() noexcept;
size_t operation_count() const noexcept;
size_t size() const noexcept;
bool empty() const noexcept;
```

履歴容量、現在の履歴操作数、現在queueの要素数、現在queueが空かを返す。

## `top`, `values`

```cpp
const T& top() const;
vector<T> values() const;
```

`top()` は現在最優先の要素を返す。`values()` は現在queueの全要素を `Compare` 順に返す。

## `contains_time`, `operation_kind`, `pushed_value`

```cpp
bool contains_time(const Time& time) const;
RetroactivePriorityQueueOperationKind operation_kind(const Time& time) const;
const T& pushed_value(const Time& time) const;
```

指定時刻の操作の有無・種類・push値を取得する。`pushed_value` はpush操作にだけ使用できる。

## `insert_push`, `insert_pop`

```cpp
void insert_push(const Time& time, const T& value);
void insert_pop(const Time& time);
```

過去または現在に新しい操作を挿入する。時刻は履歴内で一意でなければならない。`insert_pop` は、挿入後のどのprefixでもqueueサイズが負にならない場合だけ成功する。

## `erase`

```cpp
void erase(const Time& time);
```

指定時刻の操作を削除する。push削除によって後続popが空queueへ適用される場合など、削除後の履歴が不正なら変更を拒否する。

## `clear`

```cpp
void clear() noexcept;
```

全履歴と現在queueを空にする。

## 時間計算量

編集前の履歴長を $M$、現在queueの要素数を $H$ とする。時刻treapの高さに対する期待計算量を示す。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| constructor | $O(\mathrm{MAX\_OPERATION})$ | object内に $O(\mathrm{MAX\_OPERATION})$ |
| `capacity`, `operation_count`, `size`, `empty`, `top` | $O(1)$ | $O(1)$ |
| `contains_time`, `operation_kind`, `pushed_value` | 期待 $O(\log M)$ | $O(1)$ |
| `insert_push`, `insert_pop`, `erase` | 期待 $O(\log M)$ | 期待 $O(\log M)$ stack |
| `values` | $O(M+H\log H)$ | $O(H+\log M)$ |
| `clear` | $O(M)$ | 期待 $O(\log M)$ stack |

## 注意点

- 同じ時刻への2操作目、存在しない時刻の削除・参照、空queueの `top` は `runtime_error` を送出する。
- 操作数が `MAX_OPERATION` に達した後の挿入は `runtime_error` を送出する。
- 時刻順のどこかで空queueへpopする履歴は不正である。不正になる挿入・削除は `runtime_error` を送出し、履歴と現在状態を変更しない。
- `operation_kind` にない時刻、`pushed_value` にない時刻またはpop時刻を渡すと `runtime_error` を送出する。
- `Compare` と `TimeCompare` はstrict weak orderingを満たし、比較中に例外を送出してはならない。
- 同値な値のpop対象は区別しない。現在queueはmultisetとして重複を保持する。

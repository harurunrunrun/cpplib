---
title: Partially Retroactive Priority Queue (部分遡及優先度付きキュー)
documentation_of: ../../../../src/structure/heap/priority_queue/partially_retroactive_priority_queue.hpp
---

優先度付きキューへの push・pop の操作を時刻順に保持し、過去の時刻への操作の追加と操作の削除を反映した現在の状態を扱う。問い合わせの対象が現在の状態だけなので、部分遡及（partially retroactive）である。

```cpp
template<
    class Time,
    class T,
    size_t MAX_OPERATION = SIZE_MAX,
    class Compare = less<T>,
    class TimeCompare = less<Time>
>
class PartiallyRetroactivePriorityQueue;
```

`Compare` で最も小さい要素を `pop`・`top` の対象とする。デフォルトの `less<T>` では最小値を取り出すキューになる。時刻が同じかどうかは `TimeCompare` による同値で判定する。
`MAX_OPERATION` を省略（`SIZE_MAX`）すると、実行時の操作数に合わせて `std::vector` を確保し、解放したノードを再利用する可変長版になる。

```cpp
using Dynamic = PartiallyRetroactivePriorityQueue<int, long long>;
```

内部では、現在まで残る push を active、pop 済みの push を inactive として管理する。時刻の treap に、橋渡し（bridge）用の接頭辞の集約、キューのサイズの接頭辞最小値、active な値の最小値、inactive な値の最大値を保持するため、pop の対象が連鎖的に変わる場合も履歴を再実行しない。

## 操作の種類

```cpp
enum class RetroactivePriorityQueueOperationKind {
    push,
    pop
};
```

## コンストラクタ

```cpp
PartiallyRetroactivePriorityQueue(
    Compare compare = Compare{},
    TimeCompare time_compare = TimeCompare{}
);
```

空の履歴と空の現在のキューを作る。

## capacity / operation_count / size / empty

```cpp
static constexpr size_t capacity() noexcept;  // 可変長版では非静的で、確保済みのノード数を返す
size_t operation_count() const noexcept;
size_t size() const noexcept;
bool empty() const noexcept;
```

履歴の容量、現在の履歴の操作数、現在のキューの要素数、現在のキューが空かどうかを返す。

## top / values

```cpp
const T& top() const;
vector<T> values() const;
```

`top()` は現在最も優先される要素を返す。`values()` は現在のキューの全要素を `Compare` の順に返す。

## contains_time / operation_kind / pushed_value

```cpp
bool contains_time(const Time& time) const;
RetroactivePriorityQueueOperationKind operation_kind(const Time& time) const;
const T& pushed_value(const Time& time) const;
```

指定した時刻の操作の有無・種類・push した値を取得する。`pushed_value` は push の操作にだけ使える。

## insert_push / insert_pop

```cpp
void insert_push(const Time& time, const T& value);
void insert_pop(const Time& time);
```

過去または現在に新しい操作を挿入する。時刻は履歴内で一意でなければならない。`insert_pop` は、挿入後のどの接頭辞でもキューのサイズが負にならない場合だけ成功する。

## erase

```cpp
void erase(const Time& time);
```

指定した時刻の操作を削除する。push の削除によって後続の pop が空のキューに適用されるなど、削除後の履歴が不正になる場合は変更を拒否する。

## clear

```cpp
void clear() noexcept;
```

全履歴と現在のキューを空にする。

## 時間計算量

編集前の履歴の長さを $M$、現在のキューの要素数を $H$ とする。時刻の treap の高さに対する期待計算量を示す。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | 固定長版 $O(\texttt{MAX_OPERATION})$、可変長版 $O(1)$ | 固定長版はオブジェクト内に $O(\texttt{MAX_OPERATION})$ |
| `capacity`, `operation_count`, `size`, `empty`, `top` | $O(1)$ | $O(1)$ |
| `contains_time`, `operation_kind`, `pushed_value` | 期待 $O(\log M)$ | $O(1)$ |
| `insert_push`, `insert_pop`, `erase` | 期待 $O(\log M)$（可変長版は期待償却） | 期待 $O(\log M)$ のスタック |
| `values` | $O(M+H\log H)$ | $O(H+\log M)$ |
| `clear` | $O(M)$ | 期待 $O(\log M)$ のスタック |

## 空間計算量

固定長版は $O(\texttt{MAX_OPERATION})$、可変長版は同時に保持した操作数の最大値を $m$ として $O(m)$。

## 注意点

- 同じ時刻への 2 つ目の操作、存在しない時刻の削除・参照、空のキューへの `top` は `std::runtime_error` を送出する。
- 固定長版で、操作数が `MAX_OPERATION` に達した後の挿入は `std::runtime_error` を送出する。
- 時刻順のどこかで空のキューに pop する履歴は不正である。不正になる挿入・削除は `std::runtime_error` を送出し、履歴と現在の状態を変更しない。
- `operation_kind` に存在しない時刻を、`pushed_value` に存在しない時刻または pop の時刻を渡すと `std::runtime_error` を送出する。
- `Compare` と `TimeCompare` は狭義の弱順序を満たし、比較中に例外を送出してはならない。
- 同値な値どうしは pop の対象として区別しない。現在のキューは多重集合として重複を保持する。
- 可変長版で返された値への参照は、後続の更新に伴う再確保で無効になることがある。必要な値は更新前にコピーする。

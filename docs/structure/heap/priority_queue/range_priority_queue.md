---
title: Range Priority Queue (区間優先度付きキュー)
documentation_of: ../../../../src/structure/heap/priority_queue/range_priority_queue.hpp
---

複数の優先度付きキューを添字順に保持し、キューの半開区間 $[l, r)$ に含まれる全要素のうち `Compare` の上で最大の要素を取得・削除する。
各キューの先頭をセグメント木に載せるため、計算量は区間の幅に依存しない。

## テンプレート引数

```cpp
template<class T, class Compare = std::less<T>>
struct RangePriorityQueue;
```

- `T`: 要素の型
- `Compare`: 狭義の弱順序。`std::priority_queue` と同様に、`Compare` の上で最大の要素を優先する

## コンストラクタ

```cpp
explicit RangePriorityQueue(
    std::size_t bucket_count,
    const Compare& compare = Compare()
)
```

空のキューを `bucket_count` 個作る。以下、キューの個数を $B$ とする。

## size

```cpp
std::size_t size() const
```

キューの個数 $B$ を返す。

## empty

```cpp
bool empty(std::size_t index) const
```

`index` 番目のキューが空かどうかを返す。

## bucket_element_count

```cpp
std::size_t bucket_element_count(std::size_t index) const
```

`index` 番目のキューの要素数を返す。

## range_empty

```cpp
bool range_empty(std::size_t left, std::size_t right) const
```

$[\mathrm{left}, \mathrm{right})$ にあるキューがすべて空かどうかを返す。空区間に対しては `true` を返す。

## push

```cpp
void push(std::size_t index, const T& value)
void push(std::size_t index, T&& value)
```

`index` 番目のキューに `value` を追加する。

## bucket_top

```cpp
const T& bucket_top(std::size_t index) const
```

`index` 番目のキューの先頭を返す。

## top

```cpp
T top(std::size_t left, std::size_t right) const
```

$[\mathrm{left}, \mathrm{right})$ 内の全キューにある要素の最大値を返す。

## top_with_index

```cpp
std::pair<T, std::size_t> top_with_index(
    std::size_t left,
    std::size_t right
) const
```

最大値と、それを保持するキューの添字を返す。同値な最大値が複数のキューにある場合は、最小の添字を選ぶ。

## pop

```cpp
T pop(std::size_t left, std::size_t right)
```

$[\mathrm{left}, \mathrm{right})$ 内の最大値を削除して返す。

## pop_bucket

```cpp
T pop_bucket(std::size_t index)
```

`index` 番目のキューの先頭を削除して返す。

## 時間計算量

対象のキューの（操作前の）要素数を $S$ とする。

- コンストラクタ: $O(B)$
- `size`、`empty`、`bucket_element_count`、`bucket_top`: $O(1)$
- `range_empty`、`top`、`top_with_index`: $O(\log(B+1))$
- `push`: $O(\log(S+1)+\log(B+1))$（追加領域は償却 $O(1)$）
- `pop`、`pop_bucket`: $O(\log S+\log(B+1))$（追加領域 $O(1)$）

## 空間計算量

全要素数を $N$ として、要素の保存領域は $O(N)$、セグメント木とキューの管理領域は $O(B)$。

## 注意点

- 添字は $0 \le \mathrm{index} \lt B$、区間は $0 \le \mathrm{left} \le \mathrm{right} \le B$ を満たす必要がある。違反すると `std::runtime_error` を送出する。
- 空のキューへの `bucket_top`、`pop_bucket` と、対象区間が空のときの `top`、`top_with_index`、`pop` は `std::runtime_error` を送出する。

---
title: Range Priority Queue (区間優先度付きキュー)
documentation_of: ../src/structure/other/range_priority_queue.hpp
---

複数の priority queue を添字順に保持し、queue の半開区間 $[l,r)$ に含まれる
全要素のうち `Compare` 上で最大の要素を取得・削除する。各 queue の先頭を
segment tree に載せるため、区間の幅には依存しない。

## テンプレート引数

```cpp
template<class T, class Compare = std::less<T>>
struct RangePriorityQueue;
```

- `T`: 要素型。
- `Compare`: strict weak ordering。`std::priority_queue` と同様、`Compare` 上で最大の
  要素を優先する。

## 構築

```cpp
explicit RangePriorityQueue(
    std::size_t bucket_count,
    const Compare& compare = Compare()
)
```

空の queue を `bucket_count` 個作る。

- 時間計算量: $O(B)$
- 空間計算量: $O(B)$

## `size`

```cpp
std::size_t size() const
```

queue の個数 $B$ を返す。

- 時間計算量: $O(1)$

## `empty`

```cpp
bool empty(std::size_t index) const
```

`index` 番目の queue が空かを返す。

- 時間計算量: $O(1)$

## `bucket_element_count`

```cpp
std::size_t bucket_element_count(std::size_t index) const
```

`index` 番目の queue の要素数を返す。

- 時間計算量: $O(1)$

## `range_empty`

```cpp
bool range_empty(std::size_t left, std::size_t right) const
```

$[left,right)$ にある queue がすべて空かを返す。空区間に対しては `true`。

- 時間計算量: $O(\log B)$

## `push`

```cpp
void push(std::size_t index, const T& value)
void push(std::size_t index, T&& value)
```

`index` 番目の queue に `value` を追加する。その queue の要素数を $S$ とする。

- 時間計算量: $O(\log(S+1)+\log(B+1))$
- 追加空間計算量: 償却 $O(1)$

## `bucket_top`

```cpp
const T& bucket_top(std::size_t index) const
```

`index` 番目の queue の先頭を返す。空なら `runtime_error`。

- 時間計算量: $O(1)$

## `top`

```cpp
T top(std::size_t left, std::size_t right) const
```

$[left,right)$ 内の全 queue にある要素の最大値を返す。対象が空なら
`runtime_error`。

- 時間計算量: $O(\log B)$

## `top_with_index`

```cpp
std::pair<T, std::size_t> top_with_index(
    std::size_t left,
    std::size_t right
) const
```

最大値と、それを保持する queue の添字を返す。同値な最大値が複数の queue に
ある場合は最小の添字を選ぶ。対象が空なら `runtime_error`。

- 時間計算量: $O(\log B)$

## `pop`

```cpp
T pop(std::size_t left, std::size_t right)
```

$[left,right)$ 内の最大値を削除して返す。対象が空なら `runtime_error`。
削除元 queue の要素数を $S$ とする。

- 時間計算量: $O(\log S+\log(B+1))$
- 追加空間計算量: $O(1)$

## `pop_bucket`

```cpp
T pop_bucket(std::size_t index)
```

`index` 番目の queue の先頭を削除して返す。空なら `runtime_error`。
削除前の queue の要素数を $S$ とする。

- 時間計算量: $O(\log S+\log(B+1))$
- 追加空間計算量: $O(1)$

## 注意点

添字は $0\le index<B$、区間は $0\le left\le right\le B$ を満たす必要がある。
違反時は `runtime_error`。要素全体の保存領域は $O(N)$、segment tree と queue
管理領域は $O(B)$。


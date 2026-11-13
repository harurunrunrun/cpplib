---
title: XOR MEX Set (XOR一括変換MEX集合)
documentation_of: ../../../../src/structure/trie/binary/xor_mex_set.hpp
---

固定ビット幅の整数の多重集合を管理し、全要素に XOR を一括適用した後の MEX を求める二分トライである。

```cpp
XorMexSet<BitWidth = 30>
```

## `insert` / `erase`

```cpp
bool insert(uint64_t logical_value)
bool erase(uint64_t logical_value)
```

現在の（XOR 適用後の）論理値を挿入・削除する。`insert` は値が初めて現れたかを、`erase` は 1 個を実際に削除できたかを返す。

## `apply_xor` / `mex`

```cpp
void apply_xor(uint64_t value)
uint64_t mex() const
```

`apply_xor` は全論理値に `value` との XOR を遅延適用する。`mex` は現在の状態の MEX を返し、値域 $[0, 2^{B})$ のすべての値が存在すれば `universe()` を返す。

## `universe` / `distinct_size` / `xor_mask`

```cpp
static constexpr uint64_t universe()
uint64_t distinct_size() const
uint64_t xor_mask() const
```

値域の大きさ $2^{B}$、相異なる値の個数、累積 XOR のマスクを返す。

## 時間計算量

ビット幅 `BitWidth` を $B$、生成したノード数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `insert` | $O(B)$ | 償却 $O(B)$ |
| `erase`, `mex` | $O(B)$ | $O(1)$ |
| `apply_xor`、その他の取得関数 | $O(1)$ | $O(1)$ |
| 全体 | - | $O(K)$ |

## 注意点

- `1 <= BitWidth <= 62` でなければならない。
- `insert`、`erase`、`apply_xor` の引数は $[0, 2^{B})$ を要求し、違反時は `out_of_range` を送出する。
- 同じ値を複数挿入しても、MEX の計算では一度だけ数える。

---
title: XOR MEX Set (XOR一括変換MEX集合)
documentation_of: ../../../../src/structure/trie/binary/xor_mex_set.hpp
---

固定ビット幅の重複付き整数集合を管理し、全要素への累積 XOR 後の MEX を求める二分 trie である。

## insert / erase

```cpp
bool insert(uint64_t logical_value)
bool erase(uint64_t logical_value)
```

現在見えている論理値を挿入・削除する。`insert` は値が初出現だったか、`erase` は 1 個を実際に削除できたかを返す。

## apply_xor / mex

```cpp
void apply_xor(uint64_t value)
uint64_t mex() const
```

全論理値に XOR を遅延適用し、その状態の MEX を返す。領域が完全なら `universe()` を返す。

## universe / distinct_size / xor_mask

```cpp
static constexpr uint64_t universe()
uint64_t distinct_size() const
uint64_t xor_mask() const
```

領域長、異なる値数、累積 XOR mask を返す。

## 時間計算量

ビット幅を $B$、生成 node 数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `insert` | $O(B)$ | 償却 $O(B)$ |
| `erase`, `mex` | $O(B)$ | $O(1)$ |
| `apply_xor`, その他 getter | $O(1)$ | $O(1)$ |
| 全体 | - | $O(K)$ |

## 注意点

`1 <= BitWidth <= 62`。値は `[0,2^BitWidth)` を要求し、違反時は `out_of_range` を送出する。同じ値を複数挿入しても MEX 上は一度だけ数える。

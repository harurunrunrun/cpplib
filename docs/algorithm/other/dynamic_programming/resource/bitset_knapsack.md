---
title: Bitset Knapsack (bitsetナップサック)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/bitset_knapsack.hpp
---

各品物を高々 1 回使うときに作れる部分和の集合を、64 ビットの語単位のビット列で 64 個ずつ並列に更新する。

## API

```cpp
BitsetKnapsack knapsack(capacity);
```

到達可能な和を $0$ 以上 `capacity` 以下に制限して初期化する。最初は $0$ だけが到達可能である。

```cpp
void knapsack.add(weight);
```

非負の重みの品物を 1 個追加する。

```cpp
bool knapsack.reachable(weight) const;
int knapsack.maximum_reachable() const;
```

指定した和に到達可能かどうかと、容量以下で到達可能な和の最大値を返す。`reachable` は範囲外の値に対して `false` を返す。

## 時間計算量

容量を $W$ とする。

- 構築: $O(W/64)$
- `add`: $O(W/64)$
- `reachable`: $O(1)$
- `maximum_reachable`: $O(W/64)$

## 空間計算量

$O(W/64)$。

## 注意点

- 負の容量または負の重みでは `std::runtime_error` を送出する。
- 容量を超える品物と重み $0$ の品物は、到達可能な和の集合を変えない。

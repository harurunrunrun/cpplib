---
title: Bitset Knapsack (bitsetナップサック)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/bitset_knapsack.hpp
---

各品物を高々1回使う部分和到達可能性を64個ずつ並列に更新する。

## API

```cpp
BitsetKnapsack knapsack(capacity);
```

到達可能和を `0..capacity` に制限して初期化する。最初は `0` だけが到達可能。

```cpp
void knapsack.add(weight);
```

非負重みの品物を1個追加する。

```cpp
bool knapsack.reachable(weight) const;
int knapsack.maximum_reachable() const;
```

指定和の到達可能性と、容量以下の最大到達可能和を返す。

## 時間計算量

容量を $W$ とする。

- 構築: $O(W/64)$
- `add`: $O(W/64)$
- `reachable`: $O(1)$
- `maximum_reachable`: $O(W/64)$

## 空間計算量

$O(W/64)$。

## 注意点

負容量または負重みでは `runtime_error` を送出する。容量を超える品物と重み `0` の品物は到達可能集合を変えない。

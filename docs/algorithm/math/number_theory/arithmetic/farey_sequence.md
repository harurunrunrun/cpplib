---
title: Farey Sequence (Farey数列)
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/farey_sequence.hpp
---

分母が $N$ 以下の $[0,1]$ 内の既約分数を昇順に列挙する。

## API

```cpp
vector<pair<int, int>> math::farey_sequence(int order);
```

各要素を `(分子, 分母)` で返し、両端 `0/1` と `1/1` を含む。

## 時間計算量

出力長を $F_N$ として $O(F_N)$ 時間・$O(F_N)$ 空間。$F_N=\Theta(N^2)$。

## 注意点

`order` は正でなければならず、違反時は `runtime_error` を送出する。

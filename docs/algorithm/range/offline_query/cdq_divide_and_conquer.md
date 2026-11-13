---
title: CDQ Divide and Conquer (CDQ分割統治)
documentation_of: ../../../../src/algorithm/range/offline_query/cdq_divide_and_conquer.hpp
---

添字区間を再帰的に二分し、左半分から右半分への寄与を後処理で計算する CDQ 分割統治の骨格。

## API

```cpp
template<class Merge>
void cdq_divide_and_conquer(int left, int right, Merge&& merge);
```

半開区間 $[\mathtt{left}, \mathtt{right})$ を `middle = left + (right - left) / 2` で分割し、左右の子を処理した後に `merge(left, middle, right)` を呼ぶ。長さ 1 以下の区間では何もしない。

## 時間計算量

`merge` が長さ $L$ の区間に対して $O(L)$ なら、全体は $O(N\log N)$（$N$ は `right - left`）。再帰の深さは $O(\log N)$。

## 注意点

- `left < 0` または `right < left` なら `invalid_argument` を送出する。
- `merge` は再帰呼び出しの間ずっと有効な呼び出し可能オブジェクトでなければならない（参照で受け渡す）。

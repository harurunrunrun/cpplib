---
title: CDQ Divide and Conquer (CDQ分割統治)
documentation_of: ../../../../src/algorithm/range/offline_query/cdq_divide_and_conquer.hpp
---

添字区間を再帰分割し、左右間の寄与を後処理するCDQ分割統治の骨格。

## API

- `cdq_divide_and_conquer(left, right, merge)`: 各区間で `merge(left,middle,right)` を子の処理後に呼ぶ。

## 時間計算量

- `merge` が長さ $L$ に対し $O(L)$ なら全体は $O(N\log N)$。

## 注意点

- `merge` は再帰呼び出し中も有効な呼び出し可能オブジェクトでなければならない。

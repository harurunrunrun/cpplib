---
title: Fischer–Heun RMQ (フィッシャー・ヒューン区間最小値)
documentation_of: ../../../../src/structure/range_query/extrema/fischer_heun_rmq.hpp
---

Cartesian 木の Euler ツアーを ±1 RMQ に帰着して、静的な列の区間最小値を求める。

## API

```cpp
template<class T, class Compare = std::less<T>>
class FischerHeunRMQ;
```

- `FischerHeunRMQ()`: 空の構造を作る。
- `explicit FischerHeunRMQ(vector<T> values, Compare compare = Compare{})`: 列を前処理する。
- `size()`: 列の長さを返す。
- `argmin(left, right)`: 半開区間 `[left, right)` で `compare` の意味で最小の要素の添字を返す。最小の要素が複数あれば最も左の添字を返す。
- `minimum(left, right)`: 最小の要素への const 参照を返す。

## 時間計算量

列の長さを $N$ とする。

- 構築: $O(N\log N)$（内部の ±1 RMQ のブロック表はハッシュテーブルで共有する）
- `size`, `argmin`, `minimum`: $O(1)$

空間計算量は $O(N)$ である。

## 注意点

- クエリ区間は空でない半開区間とする。`0 <= left < right <= size()` を満たさなければ `std::out_of_range` を送出する。

---
title: Fischer–Heun RMQ (フィッシャー・ヒューン区間最小値)
documentation_of: ../../../../src/structure/range_query/extrema/fischer_heun_rmq.hpp
---

Cartesian木のEuler巡回を±1 RMQへ帰着して静的区間最小値を取得する。

## API

- `FischerHeunRMQ(values,compare)`: 列を前処理する。
- `size()`: 長さを返す。
- `argmin(left,right)`: `compare` で最小の左端添字を返す。
- `minimum(left,right)`: 最小要素への参照を返す。

## 時間計算量

- 構築: $O(N\log N)$。
- `size`, `argmin`, `minimum`: $O(1)$。

## 注意点

- クエリ区間は空でない半開区間とする。

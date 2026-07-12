---
title: Mo
documentation_of: ../src/algorithm/other/mo.hpp
---

Mo's algorithm 用のクエリ並べ替え。

区間は半開区間 $[l, r)$。

# コンストラクタ

```cpp
Mo mo(n)
```

# 関数

```cpp
mo.add_query(l, r)
mo.solve(add_left, add_right, erase_left, erase_right, answer)
mo.solve(add, erase, answer)
```

`answer(index)` には `add_query` が返した index が渡される。

## 時間計算量

- $O(Q \log Q + (N + Q)\sqrt Q)$ 回程度の add/erase

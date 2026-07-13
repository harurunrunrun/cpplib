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

## API別計算量

$N$ を列長、$Q$ をquery数、$B$ をblock sizeとする。

| API | 時間計算量 |
| --- | --- |
| `Mo(n)` | $O(1)$ |
| `add_query(l,r)` | 償却 $O(1)$ |
| `size()` | $O(1)$（metadata query） |
| `solve(..., block_size=B)` | $O(Q\log Q+QB+N^2/B)$ 回の比較・pointer移動 |
| `solve(...)`（既定block size） | $O(Q\log Q+N\sqrt Q)$ 回の比較・pointer移動 |

`answer` はちょうど $Q$ 回呼ばれる。add/erase callback 1回の計算量を $C_u$、answer callback 1回を $C_a$ とすると、既定値で $O(Q\log Q+N\sqrt Q\,C_u+Q C_a)$。query順を複製・整列する追加領域は $O(Q)$。

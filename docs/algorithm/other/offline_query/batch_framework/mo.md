---
title: Mo (Moのアルゴリズム)
documentation_of: ../../../../../src/algorithm/other/offline_query/batch_framework/mo.hpp
---

Mo's algorithm のためにクエリを並べ替え、区間の伸縮を callback で通知するクラス。区間は半開区間 $[l, r)$ で表す。

## 使い方

```cpp
Mo mo(n);
int id = mo.add_query(l, r);
mo.solve(add_left, add_right, erase_left, erase_right, answer);
mo.solve(add, erase, answer);
```

`answer(id)` には `add_query` が返したクエリ番号が渡される。

## API

- `explicit Mo(int n)`: 長さ $n$ の列を対象とする。`n < 0` なら例外を送出する。
- `int add_query(int l, int r)`: 半開区間 $[l, r)$ を登録し、0 から始まるクエリ番号を返す。$0\le l\le r\le n$ でなければならず、違反時は例外を送出する。
- `int size() const`: 登録済みクエリ数を返す。
- `solve(add_left, add_right, erase_left, erase_right, answer, block_size = -1) const`: 現在区間を動かすたびに、`add_left(i)`、`add_right(i)`、`erase_left(i)`、`erase_right(i)` を該当要素の添字で呼ぶ。`answer(id)` を呼ぶ時点で、現在区間は登録区間に正確に一致している。
- `solve(add, erase, answer, block_size = -1) const`: 左右で同じ `add(i)`、`erase(i)` を使う 3 callback 版。

`block_size > 0` ならその幅でブロックに分け、それ以外は $\max(1, \lfloor n/\sqrt{Q} \rfloor)$ を使う。`solve` は登録クエリ自体を変更しないため、繰り返し呼べる。

## 時間計算量

$N$ を列長、$Q$ をクエリ数、$B$ をブロック幅とする。

| API | 時間計算量 |
| --- | --- |
| `Mo(n)` | $O(1)$ |
| `add_query(l, r)` | 償却 $O(1)$ |
| `size()` | $O(1)$ |
| `solve(..., block_size = B)` | $O(Q\log Q+QB+N^2/B)$ 回の比較・区間端の移動 |
| `solve(...)`（デフォルトのブロック幅） | $O(Q\log Q+N\sqrt{Q})$ 回の比較・区間端の移動 |

`answer` はちょうど $Q$ 回呼ばれる。add/erase の callback 1 回の計算量を $C_u$、`answer` 1 回の計算量を $C_a$ とすると、デフォルトのブロック幅で全体は $O(Q\log Q+N\sqrt{Q}\,C_u+Q C_a)$。

## 空間計算量

クエリ順を複製・整列する追加領域として $O(Q)$。

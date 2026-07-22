---
title: Mo (Moのアルゴリズム)
documentation_of: ../src/algorithm/other/offline_query/mo.hpp
---

Mo's algorithm 用のクエリ並べ替え。

区間は半開区間 $[l, r)$。

## コンストラクタ

```cpp
Mo mo(n)
```

## 関数

```cpp
mo.add_query(l, r)
mo.solve(add_left, add_right, erase_left, erase_right, answer)
mo.solve(add, erase, answer)
```

`answer(index)` には `add_query` が返した index が渡される。

## 時間計算量

- $O(Q \log Q + (N + Q)\sqrt Q)$ 回程度の add/erase

## API別の時間計算量・空間計算量

$N$ を列長、$Q$ をquery数、$B$ をblock sizeとする。

| API | 時間計算量 |
| --- | --- |
| `Mo(n)` | $O(1)$ |
| `add_query(l,r)` | 償却 $O(1)$ |
| `size()` | $O(1)$（metadata query） |
| `solve(..., block_size=B)` | $O(Q\log Q+QB+N^2/B)$ 回の比較・pointer移動 |
| `solve(...)`（既定block size） | $O(Q\log Q+N\sqrt Q)$ 回の比較・pointer移動 |

`answer` はちょうど $Q$ 回呼ばれる。add/erase callback 1回の計算量を $C_u$、answer callback 1回を $C_a$ とすると、既定値で $O(Q\log Q+N\sqrt Q\,C_u+Q C_a)$。query順を複製・整列する追加領域は $O(Q)$。

## Callback API

- `add_query(l,r)` は半開区間 $[l,r)$ を登録し、0から始まるquery idを返す。
  $0\le l\le r\le n$ が必要で、範囲外では例外。
- 5-callback版 `solve` は現在区間を動かすたび、
  `add_left(i)`, `add_right(i)`, `erase_left(i)`, `erase_right(i)`
  を該当要素indexで呼ぶ。`answer(id)` の時点では登録区間が正確に反映済み。
- 3-callback版は左右で同じ `add(i)`, `erase(i)` を使う。
- `block_size > 0` ならその幅を使用し、それ以外はquery数から自動決定する。
  `solve` は登録query自体を変更せず、繰り返し呼べる。

## 注意点

空入力、退化入力、範囲外のindexと容量超過の扱いは各APIの説明に従う。参照・iterator・callbackを受け渡すAPIでは、変更可否とobjectの寿命を守る。

---
title: Hilbert-order Mo's Algorithm (ヒルベルト順序Mo法)
documentation_of: ../../../../src/algorithm/range/offline_query/hilbert_mo.hpp
---

区間 $[l, r)$ を点 $(l, r)$ とみなして Hilbert 曲線の順に並べ、実用上の端点の移動量を抑える Mo 法。

## API

```cpp
std::uint64_t hilbert_order(unsigned int x, unsigned int y, int bits = 21, int rotation = 0);

explicit HilbertMo(int n);
int add_query(int left, int right);
void run(add_left, add_right, remove_left, remove_right, answer) const;
```

- `hilbert_order(x, y, bits, rotation)`: $2^{\mathtt{bits}}\times 2^{\mathtt{bits}}$ の格子上での点 $(x, y)$ の Hilbert 順序の値を返す。
- `HilbertMo(n)`: 列長 $n$ を設定する。`n < 0` なら `invalid_argument` を送出する。
- `add_query(l, r)`: 半開区間 $[l, r)$ のクエリを登録し、0 から始まるクエリ番号を返す。$0\le l\le r\le n$ でなければならず、違反時は `out_of_range` を送出する。
- `run(add_left, add_right, remove_left, remove_right, answer)`: Hilbert 順に現在区間を動かし、要素の添字を引数に追加・削除のコールバックを呼ぶ。現在区間が登録区間に一致した時点で `answer(id)` を呼ぶ。登録クエリは変更しないため、繰り返し呼べる。

## 時間計算量

- `hilbert_order`: $O(\mathtt{bits})$。
- `add_query`: $O(\log N)$。`run` の並べ替えは $O(Q\log Q)$。
- 端点の移動回数は入力に依存し、典型的には通常の Mo 法と同程度以下である。

## 注意点

- 区間は半開区間 $[l, r)$ で表す。
- 追加領域はクエリ列の複製の $O(Q)$。

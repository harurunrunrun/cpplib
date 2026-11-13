---
title: Misra–Gries Algorithm (Misra–Griesアルゴリズム)
documentation_of: ../../../src/approximate/streaming/misra_gries.hpp
---

頻出要素の候補を、$C$ 個のカウンタで保持する。

## 使い方

~~~cpp
MisraGries<int> sketch(15);      // 保持するカウンタ数
sketch.add(42);
auto candidates = sketch.candidates();

MisraGries<int, 15> fixed;       // 容量をテンプレート引数で固定する版
~~~

`MisraGries<Key, Capacity, Count = std::uint64_t>` の `Capacity` を省略すると、容量 $C$ を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。キーは `==` で比較する。

## API

- `MisraGries(capacity)`（可変長版）/ `MisraGries()`（固定長版）: 全カウンタを 0 にして構築する。
- `add(key)`: 一致するカウンタを 1 増やす。なければ空き枠に挿入し、空きもなければ全カウンタを 1 減らす。
- `estimate_lower_bound(key)`: 保持中のカウンタ値を返す。保持していなければ 0。
- `candidates()`: 保持中の（キー, 頻度の下界）の組を `std::vector` で返す。
- `maximum_underestimate()`: `total() / (capacity() + 1)` を返す。
- `clear()`: 空に戻す。
- `total()`, `capacity()`: 処理した要素数、$C$ を返す。

## 時間計算量

- コンストラクタ・`add`・`estimate_lower_bound`・`candidates`・`clear`: $O(C)$
- `candidates` の戻り値は $O(C)$ 空間、その他の操作の追加空間は $O(1)$
- `total`, `capacity`, `maximum_underestimate`: $O(1)$
- 空間計算量: $O(C)$

## 注意点

- 真の頻度を $f_x$、返す下界を $\hat f_x$、ストリーム長を $N$ とすると、決定的に $0\le f_x-\hat f_x\le\lfloor N/(C+1)\rfloor$ が成り立つ。したがって $f_x>N/(C+1)$ の要素は必ず候補に残る。
- 処理数が `Count` の最大値に達した状態で `add` すると `std::overflow_error` を投げる。
- `Count` は $C+1$ を表せる整数型でなければならない。固定長版は `static_assert`、可変長版は `std::invalid_argument` で検査する（$C=0$ も `std::invalid_argument`）。

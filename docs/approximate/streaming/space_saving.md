---
title: Space-Saving (頻出要素推定)
documentation_of: ../../../src/approximate/streaming/space_saving.hpp
---

頻出要素の候補を、頻度の推定値と誤差の上界付きで $C$ 個まで保持する。

## 使い方

~~~cpp
SpaceSaving<int> sketch(16);   // 保持する候補数
sketch.add(42, 3);
auto estimate = sketch.estimate(42);

SpaceSaving<int, 16> fixed;    // 容量をテンプレート引数で固定する版
~~~

`SpaceSaving<Key, Capacity, Count = std::uint64_t>` の `Capacity` を省略すると、容量 $C$ を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。キーは `==` で比較する。

## API

- `SpaceSaving::Candidate`: キー `key`、頻度の推定値 `estimate`、過大評価幅の上界 `error` を保持する構造体。
- `SpaceSaving(capacity)`（可変長版）/ `SpaceSaving()`（固定長版）: 全枠を空にして構築する。
- `add(key, delta = 1)`: 候補の推定値に `delta` を加える。未登録で満杯なら、推定値が最小の候補を置き換える（新しい推定値は旧最小値 $+$ `delta`、`error` は旧最小値）。`delta` が 0 なら何もしない。
- `estimate(key)`: `Candidate{key, estimate, error}` を返す。保持していなければ推定値・誤差とも 0。
- `candidates()`: 推定値の降順に並べた候補を返す。
- `clear()`: 空に戻す。
- `total()`, `size()`, `capacity()`: 総重み、保持中の候補数、$C$ を返す。

## 時間計算量

- コンストラクタ・`add`・`estimate`・`clear`: $O(C)$、追加空間 $O(1)$
- `candidates`: $O(C\log C)$、戻り値を含め追加空間 $O(C)$
- `total`, `size`, `capacity`: $O(1)$
- 空間計算量: $O(C)$

## 注意点

- 候補について $\texttt{estimate}-\texttt{error}\le f_x\le\texttt{estimate}$（$f_x$ は真の頻度）が成り立つ。単位重みの更新では最小カウンタは高々 $N/C$（$N$ は総重み）なので、頻度が $N/C$ を超える要素は必ず候補に残る。
- 総重みが `Count` の上限を超える更新は `std::overflow_error` を投げる。
- `Count` は符号なし整数型でなければならない。可変長版で $C=0$ なら `std::invalid_argument`。

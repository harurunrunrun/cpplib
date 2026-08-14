---
title: Affine Sequence MEX (一次式列の時刻別MEX)
documentation_of: ../../../../src/algorithm/range/static_query/affine_sequence_mexes.hpp
---

各列 $a_i+b_i t$ が時刻 $t=1,2,\ldots,M$ に取る値の集合について、それぞれ MEX を求める。

## affine_sequence_mexes

```cpp
vector<size_t> affine_sequence_mexes(
    const vector<int64_t>& initial,
    const vector<int64_t>& slope,
    size_t step_count
)
```

MEX に関係する `[0,N]` 内の時刻・値だけを列挙する。負の傾きと傾き 0 にも対応する。

## 時間計算量

傾きが非 0 で `[0,N]` 内に入る `(i,t)` の総数を $R$ とする。傾き 0 の値は全時刻共通集合として一度だけ処理する。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `affine_sequence_mexes` | $O(N+M+R)$ | $O(N+M+R)$ |

## 注意点

`initial.size() == slope.size()` が必要で、違反時は `invalid_argument` を送出する。中間計算には 128 bit 整数を使う。

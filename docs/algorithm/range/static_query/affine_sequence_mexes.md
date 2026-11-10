---
title: Affine Sequence MEX (一次式列の時刻別MEX)
documentation_of: ../../../../src/algorithm/range/static_query/affine_sequence_mexes.hpp
---

$N$ 本の列 $a_i+b_i t$ について、各時刻 $t=1,2,\ldots,M$ に全列が取る値の集合の MEX を求める。

## affine_sequence_mexes

```cpp
vector<size_t> affine_sequence_mexes(
    const vector<int64_t>& initial,
    const vector<int64_t>& slope,
    size_t step_count
)
```

$a_i$ を `initial[i]`、$b_i$ を `slope[i]`、$M$ を `step_count` とする。
戻り値の `t - 1` 番目が時刻 $t$ における MEX である。

$N$ 本の列が取る値の MEX は $N$ 以下なので、値が $[0,N]$ に入る時刻だけを各列について列挙する。
負の傾きと傾き 0 にも対応する。傾き 0 の列の値は全時刻に共通な集合として一度だけ記録する。

## 時間計算量

傾きが非 0 で値が $[0,N]$ に入る組 $(i,t)$ の総数を $R$ とし、時刻 $t$ の答えを $\mathrm{mex}_t$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `affine_sequence_mexes` | $O(N+M+R+\sum_t \mathrm{mex}_t)$ | $O(N+M+R)$ |

各時刻の MEX は 0 から順に走査して求めるため、$\sum_t \mathrm{mex}_t$ の項が付く。
傾き 0 の列が小さい値を多く占める場合、最悪 $O(NM)$ になる。

## 注意点

`initial.size() == slope.size()` が必要で、違反時は `std::invalid_argument` を送出する。中間計算には 128 bit 整数を使う。

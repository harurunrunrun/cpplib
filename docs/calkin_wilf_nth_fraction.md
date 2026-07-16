---
title: Calkin-Wilf Nth Fraction (Calkin-Wilf木の第N分数) [NG0FRCTN]
documentation_of: ../src/algorithm/math/calkin_wilf_nth_fraction.hpp
---

Calkin-Wilf木を幅優先順に番号付けし、指定位置の既約正分数を返す。

## `calkin_wilf_nth_fraction`

```cpp
pair<uint64_t,uint64_t> calkin_wilf_nth_fraction(
    uint64_t one_based_index
);
```

根を $1/1$、$i/j$ の左子を $i/(i+j)$、右子を $(i+j)/j$ とした木の
`one_based_index` 番目を `{numerator, denominator}` で返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `calkin_wilf_nth_fraction` | $O(\log(\mathrm{one\_based\_index}))$ | $O(1)$ |

## 注意点

- `one_based_index >= 1` が必要で、違反時は `invalid_argument` を送出する。

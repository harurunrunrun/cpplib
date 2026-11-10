---
title: Monotone Minima (単調最小値)
documentation_of: ../../../../src/algorithm/other/utility/monotone_minima.hpp
---

各行の最小要素を取る列番号が行について単調非減少である行列に対し、各行の最小列を分割統治で求める。行列は要素取得関数として与えるため、全要素を保持する必要はない。同値な最小値が複数ある場合は最左の列を返す。

## `monotone_minima`

```cpp
template<class Value>
vector<int> monotone_minima(
    int row_count,
    int column_count,
    Value value
);
```

`value(row, column)` が返す値を `<` で比較し、行ごとの最小列を返す。

## 時間計算量・空間計算量

行数を $H$、列数を $W$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `monotone_minima` | $O(H + W\log H)$ 回の要素取得 | $O(H + \log H)$ |

## 注意点

- 各行の最左最小列が単調非減少でなければならない（検査しない）。
- 次元は非負で、`row_count > 0` のときは `column_count > 0` でなければならない。違反時は `invalid_argument` を送出する。
- `row_count == 0` なら空の列を返す。

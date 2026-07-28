---
title: Monotone Minima (単調最小値)
documentation_of: ../../../../src/algorithm/other/utility/monotone_minima.hpp
---

各行の最小要素を取る列番号が単調非減少である行列について、各行の最小列を求める。
行列は要素取得関数として与えるため、全要素を保持する必要はない。同値な最小値が複数ある場合は最左列を返す。

## `monotone_minima`

```cpp
template<class Value>
vector<int> monotone_minima(
    int row_count,
    int column_count,
    Value value
);
```

`value(row, column)` が返す値を比較し、行ごとの最小列を返す。

## API別の時間計算量・空間計算量

行数を `H`、列数を `W` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `monotone_minima` | $O(H + W\log H)$ 回の要素取得 | $O(H + \log H)$ |

## 注意点

- 各行の最左最小列が単調非減少でなければならない。
- `row_count > 0` のとき `column_count > 0` でなければならない。不正な次元では `invalid_argument` を送出する。

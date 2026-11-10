---
title: Sliding Window Maximum (スライド窓最大値) [ARRAYSUB]
documentation_of: ../../../../src/algorithm/range/sliding_window/sliding_window_maximum.hpp
---

固定長の各連続部分列について最大値を返す。単調 deque を用いる。

## API

~~~cpp
template<class T>
vector<T> sliding_window_maximum(
    const vector<T>& values,
    int window
)
~~~

半開区間 `[i, i + window)` の最大値を `i` の昇順に返す。
戻り値の長さは `values.size() - window + 1` である。

## 時間計算量

$N$ を入力列長、$K$ を `window` とし、比較とコピー 1 回を $O(1)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sliding_window_maximum(values, window)` | $O(N)$ | 作業領域 $O(K)$、戻り値 $O(N-K+1)$ |

## 注意点

- `1 <= window <= values.size()` が必要であり、違反時は `std::runtime_error` を送出する。
- `T` の `operator<` は狭義弱順序を定めなければならない。
- 同じ最大値が複数ある場合も、値を 1 個だけ返す。

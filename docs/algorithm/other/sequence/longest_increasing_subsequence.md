---
title: Longest Increasing Subsequence (最長増加部分列) [DPL_1_D] [longest_increasing_subsequence]
documentation_of: ../../../../src/algorithm/other/sequence/longest_increasing_subsequence.hpp
---

最長増加部分列を 1 つ復元する。返り値は元の列における添字の列である。

## 使い方

```cpp
template<class T, class Compare = std::less<T>>
vector<int> longest_increasing_subsequence(
    const vector<T>& values, bool strict = true, Compare compare = Compare());

template<class T, class Compare = std::less<T>>
int longest_increasing_subsequence_length(
    const vector<T>& values, bool strict = true, Compare compare = Compare());

auto indices = longest_increasing_subsequence(values);
auto nondecreasing = longest_increasing_subsequence(values, false);
int length = longest_increasing_subsequence_length(values);
```

`strict = true` なら狭義増加、`false` なら広義増加の部分列を扱う。第 3 引数に順序を表す比較関数を渡せる。`longest_increasing_subsequence_length` は長さだけを返す。

## 時間計算量・空間計算量

入力長を $N$、答えの長さを $L$ とし、比較関数 1 回の計算量を $O(1)$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `longest_increasing_subsequence` | $O(N\log(L+1))$ | $O(N)$ |
| `longest_increasing_subsequence_length` | $O(N\log(L+1))$ | $O(L)$ |

## 注意点

- 空列では空の添字列（長さ $0$）を返す。
- `compare` は狭義弱順序を満たさなければならない。
- `T` はコピー可能でなければならない（末尾要素の値を保持する）。

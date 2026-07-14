---
title: Distinct Subsequences Count (相異なる部分列の個数) [DSUBSEQ]
documentation_of: ../src/algorithm/string/count_distinct_subsequences.hpp
---

空部分列を含む相異なる部分列の個数を法付きで返す。

# count_distinct_subsequences

```cpp
long long count_distinct_subsequences(const string& text,
                                      long long modulus = 1000000007);
```

`text` のバイト値を文字として扱う。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_subsequences` | 期待 $O(N)$ | $O(\sigma)$ |

## 注意点

- `modulus` は正でなければならない。

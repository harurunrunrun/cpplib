---
title: Recursive Sequence Term (線形漸化式の指定項) [SEQ]
documentation_of: ../src/algorithm/math/recursive_sequence_term.hpp
---

係数が一定の線形漸化式について、1始まりの指定項を法付きで求める。

# `recursive_sequence_term`

```cpp
uint64_t recursive_sequence_term(
    const vector<uint64_t>& initial,
    const vector<uint64_t>& coefficient,
    uint64_t one_based_index,
    uint64_t modulus = 1'000'000'000
);
```

`initial[i] = a_(i+1)`、`coefficient[j]` は直前から `j+1` 個前の項に掛かる係数を表す。

## API別の時間計算量・空間計算量

次数を `K` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `recursive_sequence_term` | $O(K^2\log one\_based\_index)$ | $O(K)$ |

## 注意点

- 初期値と係数は同じ非0長で、添字と法は正でなければならない。

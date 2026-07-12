---
title: Diff
documentation_of: ../src/algorithm/string/diff.hpp
---

2 つの列の差分を LCS に基づいて求める。

# 関数

```cpp
vector<DiffOp<T>> diff_sequence(a, b)
```

`DiffOp<T>` は次を持つ。

```cpp
DiffTag tag;
T value;
```

`tag` は `Equal`, `Insert`, `Erase` のいずれか。

## 時間計算量

- $O(NM)$

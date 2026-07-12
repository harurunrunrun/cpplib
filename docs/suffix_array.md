---
title: Suffix Array
documentation_of: ../src/algorithm/string/suffix_array.hpp
---

Suffix Array と LCP Array。

## 関数

- `suffix_array(s)`
    - `std::string` と `std::vector<T>` に対応。
- `lcp_array(s, sa)`
- `lcp_array(s)`

## 計算量

- `suffix_array`: $O(n \log n)$
- `lcp_array`: $O(n)$

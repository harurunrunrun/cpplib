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
- `lcp_array(s, sa)`: $O(n)$
- `lcp_array(s)`: $O(n\log n)$（suffix array構築を含む）

## API別計算量

$N=|s|$ とする。

| API | 時間計算量 | 出力・追加領域 |
| --- | --- | --- |
| `suffix_array(const string& s)` | $O(N\log N)$ | $O(N)$ |
| `suffix_array(const vector<T>& s)` | $O(N\log N)$ | $O(N)$ |
| `lcp_array(s,sa)` | $O(N)$ | $O(N)$ |
| `lcp_array(s)` | $O(N\log N)$ | $O(N)$ |

`lcp_array(s)` は内部でsuffix arrayも構築する。各関数は長さ $N$（LCPのみ最大 $N-1$）の列を列挙して返す。

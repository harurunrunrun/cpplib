---
title: Suffix Array (接尾辞配列) [number_of_substrings] [SARRAY] [suffixarray]
documentation_of: ../src/algorithm/string/suffix_array.hpp
---

Suffix Array と LCP Array。

## 関数

- `suffix_array(s)`
    - `std::string` と `std::vector<T>` に対応。
- `lcp_array(s, sa)`
- `lcp_array(s)`
- `number_of_distinct_substrings(s)`

## 計算量の概要

- `suffix_array`: $O(n \log n)$
- `lcp_array(s, sa)`: $O(n)$
- `lcp_array(s)`: $O(n\log n)$（suffix array構築を含む）
- `number_of_distinct_substrings(s)`: $O(n\log n)$

## API別の時間計算量・空間計算量

$N=|s|$ とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `suffix_array(const string& s)` | $O(N\log N)$ | $O(N)$ |
| `suffix_array(const vector<T>& s)` | $O(N\log N)$ | $O(N)$ |
| `lcp_array(s,sa)` | $O(N)$ | $O(N)$ |
| `lcp_array(s)` | $O(N\log N)$ | $O(N)$ |
| `number_of_distinct_substrings(s)` | $O(N\log N)$ | $O(N)$ |

`lcp_array(s)` は内部でsuffix arrayも構築する。各関数は長さ $N$（LCPのみ最大 $N-1$）の列を列挙して返す。
## API詳細

- `suffix_array(s)` は `s[sa[i]:]` が辞書順になる開始位置の置換を返す。
  `string` 版はbyteを符号なしとして比較し、`vector<T>` 版は `T::operator<`
  と等値関係で座標圧縮する。空列では空を返す。
- `lcp_array(s,sa)` は長さ `max(0,N-1)` の列を返し、
  `lcp[i]` はsuffix `sa[i]` と `sa[i+1]` のLCP長。
  `sa` が長さ `N` の置換でなければ例外を送出する。
- `lcp_array(s)` はsuffix arrayを内部構築して同じLCP列を返す。
- `number_of_distinct_substrings(s)` は空でない相異なる連続部分列の個数を返す。
  全suffixの長さの総和から隣接suffixのLCP長の総和を引いて求める。空列では0。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。

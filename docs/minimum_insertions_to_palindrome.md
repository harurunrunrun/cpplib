---
title: Minimum Insertions to Palindrome (回文化の最小挿入回数) [AIBOHP]
documentation_of: ../src/algorithm/string/minimum_insertions_to_palindrome.hpp
---

列に要素を挿入して回文にするために必要な最小挿入回数を求める。
挿入位置は先頭・末尾・途中のいずれでもよく、元の列の要素の順序は変えない。

# minimum_insertions_to_palindrome

```cpp
template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence)
```

`sequence` を回文にする最小挿入回数を返す。
`sequence` とその反転列の最長共通部分列は最長回文部分列になるため、列長を $N$、その長さを $L$ とすると答えは $N-L$ である。
内部では、長さだけを2行DPで求める `longest_common_subsequence_length` を使う。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_insertions_to_palindrome(sequence)` | $O(N^2)$ | $O(N)$ |

空間計算量には反転列とLCSの2行DPを含み、$O(N^2)$ のDP表は保持しない。

## 注意点

- 空列では `0` を返し、すでに回文なら `0` を返す。
- 挿入する要素列や完成した回文そのものは復元しない。
- 要素の比較には `operator==` を使うため、文字列では大文字と小文字を区別する。
- `Sequence` はコピー、`size()`、ランダムアクセス、可変イテレータによる反転をサポートする必要がある。
- 列長は `int` に収まる必要がある。

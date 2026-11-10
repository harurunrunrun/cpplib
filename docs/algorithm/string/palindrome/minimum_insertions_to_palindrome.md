---
title: Minimum Insertions to Palindrome (回文化の最小挿入回数) [AIBOHP] [IOIPALIN]
documentation_of: ../../../../src/algorithm/string/palindrome/minimum_insertions_to_palindrome.hpp
---

列に要素を挿入して回文にするために必要な最小の挿入回数を求める。
挿入位置は先頭・末尾・途中のいずれでもよく、元の列の要素の順序は変えない。
答えは「列の長さ − 列とその反転の最長共通部分列（LCS）の長さ」である。

## API

```cpp
std::size_t minimum_insertions_to_palindrome_bytes(std::string_view sequence)
int minimum_insertions_to_palindrome(std::string_view sequence)
int minimum_insertions_to_palindrome(const std::string& sequence)
template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence)
```

## 引数・戻り値

- `minimum_insertions_to_palindrome_bytes`: バイト列についてビット並列 LCS を使い、必要な挿入回数を `std::size_t` で返す。埋め込みの NUL を含む全 256 種類のバイト値を扱える。
- `minimum_insertions_to_palindrome(std::string_view)`: バイト版を利用し、結果を `int` で返す。
- `minimum_insertions_to_palindrome(const std::string&)`: `std::string_view` 版に委譲する。
- 汎用テンプレート: 要素を `operator==` で比較できる列について、通常の LCS の DP を使う。

どの API も、元の順序を保ったまま回文にするときの最小挿入回数を返す。
空列、またはすでに回文である列では `0` を返す。

## 時間計算量・空間計算量

$N$ を `sequence.size()` とし、機械語長を 64 ビットとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_insertions_to_palindrome_bytes(sequence)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| `minimum_insertions_to_palindrome(std::string_view)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| `minimum_insertions_to_palindrome(const std::string&)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| 汎用 `minimum_insertions_to_palindrome(sequence)` | $O(N^2)$ | $O(N)$ |

## 注意点

- 挿入する要素や完成した回文そのものは復元しない。
- バイト版は入力を変更せず、反転したコピーを内部で 1 つ作る。
- `int` を返す 2 つのオーバーロードは、列長が `int` に収まらない場合に `std::length_error` を送出する。
- 汎用テンプレートの `Sequence` はコピー、`size()`、ランダムアクセス、可変イテレータによる反転に対応し、列長は `int` に収まる必要がある。

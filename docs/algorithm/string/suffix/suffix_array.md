---
title: Suffix Array (接尾辞配列) [SARRAY] [suffixarray]
documentation_of: ../../../../src/algorithm/string/suffix/suffix_array.hpp
---

接尾辞配列（Suffix Array）と LCP 配列を構築する。

## API

```cpp
std::vector<int> suffix_array(const std::string& s)
template<class T>
std::vector<int> suffix_array(const std::vector<T>& s)
template<class Sequence>
std::vector<int> lcp_array(const Sequence& s, const std::vector<int>& sa)
template<class Sequence>
std::vector<int> lcp_array(const Sequence& s)
```

- `suffix_array(s)` は、`s[sa[i]:]` が辞書順に並ぶような開始位置の置換を返す。
  `std::string` 版はバイトを符号なしとして比較し、`std::vector<T>` 版は `T` の `operator<` と等値関係で座標圧縮する。
  空列では空の列を返す。
  バイトのアルファベットが定数個である `std::string` 版は、induced sorting と LMS 部分文字列の再帰的な圧縮を行う SA-IS を使う。
  一般列版は座標圧縮の後に同じ SA-IS を使う。
- `lcp_array(s, sa)` は長さ `max(0, N - 1)` の列を返し、`lcp[i]` は接尾辞 `sa[i]` と `sa[i+1]` の LCP 長である。
  `sa` が長さ $N$ の置換でなければ `std::runtime_error` を送出する。
- `lcp_array(s)` は接尾辞配列を内部で構築し、同じ LCP 列を返す。

## API 別の時間計算量・空間計算量

$N = \lvert s\rvert$ とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `suffix_array(const std::string& s)` | $O(N)$ | $O(N)$ |
| `suffix_array(const std::vector<T>& s)` | $O(N\log N)$（座標圧縮のソート。接尾辞の整列は SA-IS で $O(N)$） | $O(N)$ |
| `lcp_array(s, sa)` | $O(N)$ | $O(N)$ |
| `lcp_array(const std::string& s)` | $O(N)$ | $O(N)$ |
| `lcp_array(const std::vector<T>& s)` | $O(N\log N)$（座標圧縮のソート） | $O(N)$ |

## 注意点

- 返す位置はすべて 0-indexed である。
- 対応する入力型は `std::string` と `std::vector<T>` のみである。

---
title: Longest Common Suffix Dictionary (最長共通接尾辞辞書) [PRHYME]
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_suffix_dictionary.hpp
---

登録された文字列の中から、クエリとの共通接尾辞が最長となる別の文字列を検索する。
最長の候補が複数ある場合は、辞書順最小の文字列を返す。

## API

### コンストラクタ / `build(dictionary)`

```cpp
LongestCommonSuffixDictionary();
explicit LongestCommonSuffixDictionary(std::vector<std::string> dictionary);
void build(std::vector<std::string> dictionary);
```

`dictionary` を所有して検索構造を構築する。`build` は以前の内容をすべて置き換える。
空の辞書も構築できる。

### `size()` / `empty()`

```cpp
int size() const;
bool empty() const;
```

登録数と、辞書が空かどうかを返す。

### `best_match(query)`

```cpp
std::optional<std::string_view> best_match(std::string_view query) const;
```

`query` と完全に一致する文字列を除いた登録文字列のうち、`query` との共通接尾辞が最長となるものの中で辞書順最小の候補を返す。
候補がなければ `std::nullopt` を返す。

## API 別の時間計算量・空間計算量

登録数を $N$、登録文字列の最大長を $M$、総文字数を $S$、$L = \lvert \mathtt{query}\rvert$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルトコンストラクタ | $O(1)$ | $O(1)$ |
| `LongestCommonSuffixDictionary(dictionary)` | $O(S+NM\log(N+1))$ | $O(S+N)$ |
| `build(dictionary)` | $O(S+NM\log(N+1))$ | $O(S+N)$ |
| `size`, `empty` | $O(1)$ | $O(1)$ |
| `best_match(query)` | $O((L+1)\log(N+1))$ | $O(L)$ |

## 注意点

- 登録文字列は互いに異なる必要がある。重複があると `std::invalid_argument` を送出する。
- 文字の種類に制限はなく、空文字列も登録・検索できる。
- 登録数、各登録文字列の長さ、`query` の長さは `int` に収まる必要がある。超える場合は `std::length_error` を送出する。
- 戻り値の `std::string_view` は内部の文字列を参照する。`build` の再呼び出し、オブジェクトの破棄、または非 const 操作の後は使用できない。

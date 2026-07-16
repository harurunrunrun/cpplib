---
title: Longest Common Suffix Dictionary (最長共通接尾辞辞書) [PRHYME]
documentation_of: ../src/algorithm/string/longest_common_suffix_dictionary.hpp
---

登録文字列から、クエリと最長の共通接尾辞を持つ別の文字列を検索します。
最長候補が複数ある場合は辞書順最小の文字列を返します。

## API

### コンストラクタ / `build(dictionary)`

```cpp
LongestCommonSuffixDictionary();
explicit LongestCommonSuffixDictionary(vector<string> dictionary);
void build(vector<string> dictionary);
```

`dictionary` を所有して検索構造を構築します。`build` は以前の内容を
すべて置き換えます。空の辞書も構築できます。

### `size()` / `empty()`

```cpp
int size() const;
bool empty() const;
```

登録数と、辞書が空かどうかを返します。

### `best_match(query)`

```cpp
optional<string_view> best_match(string_view query) const;
```

`query` と共通接尾辞が最長となる登録文字列のうち、`query` と完全一致する
文字列を除いた辞書順最小の候補を返します。候補がなければ `nullopt` です。

## API別の時間計算量・空間計算量

登録数を $N$、登録文字列の最大長を $M$、総文字数を $S$、
$L=|query|$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルトコンストラクタ | $O(1)$ | $O(1)$ |
| `LongestCommonSuffixDictionary(dictionary)` | $O(S+NM\log(N+1))$ | $O(S+N)$ |
| `build(dictionary)` | $O(S+NM\log(N+1))$ | $O(S+N)$ |
| `size`, `empty` | $O(1)$ | $O(1)$ |
| `best_match(query)` | $O((L+1)\log(N+1))$ | $O(L)$ |

## 注意点

- 登録文字列は互いに異なる必要があります。重複には `invalid_argument` を
  送出します。
- 文字集合に制限はなく、空文字列も登録・検索できます。
- 登録数と各文字列長は `int` に収まる必要があります。
- 戻り値の `string_view` は内部文字列を参照します。`build` の再呼び出し、
  オブジェクトの破棄、または非const操作の後は使用できません。

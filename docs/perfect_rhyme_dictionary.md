---
title: Perfect Rhyme Dictionary (最適共通接尾辞辞書) [PRHYME]
documentation_of: ../src/algorithm/string/perfect_rhyme_dictionary.hpp
---

辞書中の語から、クエリ語と最長の共通接尾辞を持つ別の語を検索する。
候補が複数ある場合は辞書順最小の語を返す。

内部では各語を反転して整列し、共通接尾辞を共通接頭辞として扱う。
同じ接頭辞を持つ区間に対して、元の辞書順で最小の2語を区間木で管理する。

# コンストラクタ / build

```cpp
PerfectRhymeDictionary()
PerfectRhymeDictionary(vector<string> dictionary)
void build(vector<string> dictionary)
```

`dictionary` をコピーまたはムーブして辞書を構築する。
`build` は以前の内容をすべて置き換える。空の辞書も構築できる。

# size / empty

```cpp
int size() const
bool empty() const
```

登録語数と、辞書が空かどうかを返す。

# perfect_rhyme

```cpp
optional<string_view> perfect_rhyme(string_view word) const
```

`word` と共通接尾辞が最長となる登録語のうち、`word` 自身と同じ文字列を除いた辞書順最小の語を返す。
候補が存在しない場合は `nullopt` を返す。返された `string_view` は辞書が所有する文字列を参照する。

## API別の時間計算量・空間計算量

$N$ を登録語数、$M$ を登録語の最大長、$S$ を登録語の総文字数、$L=|word|$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| デフォルトコンストラクタ | $O(1)$ | $O(1)$ |
| `PerfectRhymeDictionary(dictionary)` | $O(S+NM\log (N+1))$ | $O(S+N)$ |
| `build(dictionary)` | $O(S+NM\log (N+1))$ | $O(S+N)$ |
| `size`, `empty` | $O(1)$ | $O(1)$ |
| `perfect_rhyme(word)` | $O(1+(L+1)\log (N+1))$ | $O(L)$ |

構築時の計算量には、元の語と反転した語の辞書順整列を含む。

## 注意点

- 登録語は互いに異なる必要がある。重複があれば例外。
- 文字集合に制限はなく、空文字列も登録・検索できる。
- `word` が辞書に存在する場合、その1語だけを候補から除外する。
- 共通接尾辞長が0の語も候補であり、その場合も辞書順最小を返す。
- `build` の再呼び出し、オブジェクトの破棄、または非const操作の後は、以前に返した `string_view` を使用しないこと。

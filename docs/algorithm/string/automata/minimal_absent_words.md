---
title: Minimal Absent Words (極小不在語)
documentation_of: ../../../../src/algorithm/string/automata/minimal_absent_words.hpp
---

文字列に出現しないが、その真の部分文字列はすべて出現する語（極小非出現語）を、指定したアルファベット上で列挙する。
接尾辞オートマトンの各状態と、その接尾辞リンク先の状態との遷移の差を調べる。

## API

```cpp
std::vector<std::string> minimal_absent_words(
    std::string_view text, std::vector<unsigned char> alphabet
)
std::vector<std::string> minimal_absent_words(
    std::string_view text, std::string_view alphabet
)
```

返り値はバイト単位の辞書順に並び、重複を除いている。`alphabet` 内の重複も除去する。

## 時間計算量・空間計算量

$N$ を `text` の長さ、$\Sigma$ を `alphabet` の大きさ、返す語の文字数の合計を $Z$ とする。
接尾辞オートマトンは各状態に長さ 256 の遷移表を持つ。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimal_absent_words` | $O(256N+\Sigma N+Z\log Z)$ | $O(256N+Z)$ |

## 注意点

`alphabet` に含まれないバイトを使う語は列挙の対象外である。

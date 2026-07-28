---
title: Minimal Absent Words (極小不在語)
documentation_of: ../../../../src/algorithm/string/automata/minimal_absent_words.hpp
---

文字列に出現しないが、真の部分文字列はすべて出現する語を指定alphabet上で列挙する。
suffix automaton の状態とsuffix link間の遷移差を調べる。

## API

```cpp
std::vector<std::string> minimal_absent_words(
    std::string_view text, std::vector<unsigned char> alphabet
)
std::vector<std::string> minimal_absent_words(
    std::string_view text, std::string_view alphabet
)
```

返り値はbyte辞書順で重複を除いている。alphabet内の重複も除去する。

## API別の時間計算量・空間計算量

$N=|text|,\ \Sigma=|alphabet|$、返す語の総文字数を $Z$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimal_absent_words` | $O(256N+\Sigma N+Z\log Z)$ | $O(256N+Z)$ |

## 注意点

alphabetに含まれないbyteを使う語は列挙対象外である。

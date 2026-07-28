---
title: Prefix Automaton (接頭辞オートマトン)
documentation_of: ../../../../src/algorithm/string/automata/prefix_automaton.hpp
---

パターンのKMP接頭辞関数から、全256 byte に対する決定性オートマトンを構築する。

## API

```cpp
PrefixAutomaton(std::string pattern)
int next_state(int state, unsigned char symbol) const
std::vector<int> match_positions(std::string_view text) const
```

`next_state` は1文字読んだ後の一致接頭辞長を返す。`match_positions` は
パターンが始まる0-indexed位置を列挙し、空パターンでは $0,\ldots,|text|$ を返す。

## API別の時間計算量・空間計算量

$M=|pattern|,\ N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(256M)$ | $O(256M)$ |
| `next_state` | $O(1)$ | $O(1)$ |
| `match_positions` | $O(N+K)$ | $O(K)$ |

ここで $K$ は報告件数である。不正な状態番号では例外を送出する。

## 注意点

alphabetは全256 byteで固定され、状態番号は $0$ 以上 $M$ 以下である。

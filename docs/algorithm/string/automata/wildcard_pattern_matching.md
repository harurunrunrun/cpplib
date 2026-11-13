---
title: Wildcard Pattern Matching (ワイルドカードパターンマッチング) [wildcard_pattern_matching]
documentation_of: ../../../../src/algorithm/string/automata/wildcard_pattern_matching.hpp
---

文字列 `text` の各位置について、`pattern` がワイルドカードを考慮して一致するかを求める。
デフォルトのワイルドカードは `'*'` であり、`text` 側と `pattern` 側のどちらに現れても任意の 1 文字と一致する。

各バイトを正の整数 $a,b$ に、ワイルドカードを 0 に写すと、1 組の文字の不一致量は
$ab(a-b)^2$ で表せる。配置ごとの不一致量の総和を 3 回の畳み込みに分解し、
2 つの NTT 素数のもとで同時に 0 になるかを調べる。

## `wildcard_pattern_matching`

```cpp
std::vector<bool> result = wildcard_pattern_matching(text, pattern);
std::vector<bool> result = wildcard_pattern_matching(text, pattern, wildcard);
```

`result[i]` は、`text.substr(i, pattern.size())` と `pattern` が一致するとき `true` になる。
`pattern` が `text` より長ければ空の `std::vector` を、`pattern` が空なら `text.size() + 1` 個の `true` を返す。
`pattern` または `text` がすべてワイルドカードなら畳み込みを省略する。

## 完全性

使用する法は $998244353$ と $469762049$ である。
受理する入力では、非負整数である不一致量の総和が 2 つの法の積より小さいことを検査しているため、
両方の剰余が 0 なら整数としても 0 である。乱択は用いず、偽陽性は起こらない。

## 時間計算量・空間計算量

$N$ を `text` の長さ、$M$ を `pattern` の長さとする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `wildcard_pattern_matching` | $O((N+M)\log(N+M))$ | $O(N+M)$ |

全ワイルドカードによる早期終了時は $O(N+M)$ 時間である。

## 注意点

- 必要な NTT 長が $2^{23}$ を超える場合、または上記の完全性の上限を超える場合は `std::length_error` を送出する。
- 文字はバイトとして扱う。返す添字は 0-indexed である。

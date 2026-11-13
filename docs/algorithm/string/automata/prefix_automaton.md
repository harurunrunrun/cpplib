---
title: Prefix Automaton (接頭辞オートマトン)
documentation_of: ../../../../src/algorithm/string/automata/prefix_automaton.hpp
---

パターンの KMP 接頭辞関数から、全 256 種類のバイトに対する決定性オートマトン（KMP オートマトン）を構築する。

## API

```cpp
explicit PrefixAutomaton(std::string pattern)
int next_state(int state, unsigned char symbol) const
std::vector<int> match_positions(std::string_view text) const
```

- `next_state(state, symbol)`: 一致している接頭辞の長さが `state` の状態から 1 文字読んだ後の、一致接頭辞の長さを返す。状態 $M$（完全一致）からの遷移も定義されており、重なる出現を扱える。
- `match_positions(text)`: パターンが現れる 0-indexed の開始位置を昇順に列挙する。重なる出現も含む。空パターンでは $0,1,\ldots,\lvert \mathrm{text}\rvert$ を返す。

## 時間計算量・空間計算量

$M$ をパターン長、$N$ をテキスト長、$K$ を報告件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(256M)$ | $O(256M)$ |
| `next_state` | $O(1)$ | $O(1)$ |
| `match_positions` | $O(N+K)$ | $O(K)$ |

## 注意点

- アルファベットは全 256 種類のバイトに固定され、状態番号は $0$ 以上 $M$ 以下である。
- 範囲外の状態番号を `next_state` に渡すと `std::runtime_error` を送出する。

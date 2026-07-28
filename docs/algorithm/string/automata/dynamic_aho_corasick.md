---
title: Dynamic Aho-Corasick Automaton (動的Aho-Corasickオートマトン)
documentation_of: ../../../../src/algorithm/string/automata/dynamic_aho_corasick.hpp
---

パターンの追加・削除を、サイズが2冪の静的Aho-Corasickオートマトン群で管理する。
削除分は別のオートマトン群に加え、検索時に差し引く。

## API

```cpp
void insert(std::string pattern)
void erase(const std::string& pattern)
int pattern_count(const std::string& pattern) const
long long count_matches(std::string_view text) const
```

`count_matches` は登録中の各パターンの全出現数の総和を返す。重複登録と空パターンを
扱う。存在しないパターンの `erase` は例外を送出する。

## API別の時間計算量・空間計算量

$P$ を登録操作で渡した文字数、$Q$ を登録パターン数、$N=|text|$ とする。

| API | 償却時間計算量 | 空間計算量 |
| --- | --- | --- |
| `insert`, `erase` | $O(P\log Q\log 256)$ | $O(P\log Q)$ 一時、全体 $O(S)$ |
| `pattern_count` | 平均 $O(1)$ | $O(1)$ |
| `count_matches` | $O(N\log Q\log 256)$ | $O(1)$ |

$S$ は正負の静的オートマトンが保持する総文字数である。

## 注意点

検索結果はパターン別ではなく全登録パターンの出現数合計である。

---
title: Dynamic Aho-Corasick Automaton (動的Aho-Corasickオートマトン)
documentation_of: ../../../../src/algorithm/string/automata/dynamic_aho_corasick.hpp
---

パターンの追加・削除ができる Aho--Corasick オートマトンである。
登録パターンを、パターン数が 2 冪の静的な Aho--Corasick オートマトン群（二進カウンタ方式）で管理する。
削除したパターンは別のオートマトン群に追加し、検索時に差し引く。遷移は `std::map<unsigned char, int>` で持つ。

## API

```cpp
DynamicAhoCorasick automaton;
void insert(std::string pattern)
void erase(const std::string& pattern)
int pattern_count(const std::string& pattern) const
long long count_matches(std::string_view text) const
```

- `insert(pattern)`: パターンを 1 個登録する。同じパターンの重複登録も別々に数える。空パターンも登録できる。
- `erase(pattern)`: 登録中のパターンを 1 個削除する。登録されていないパターンを指定すると `std::runtime_error` を送出する。
- `pattern_count(pattern)`: そのパターンの現在の登録数を返す。
- `count_matches(text)`: 登録中の各パターンの `text` における全出現回数（重なりを含む）の総和を返す。

## 時間計算量・空間計算量

$P$ を 1 回の操作で渡すパターンの長さ、$Q$ をこれまでの `insert` と `erase` の総回数、$T$ をテキスト長とする。
遷移の探索は `std::map` によるため、1 回あたり $O(\log 256)=O(1)$ とみなせるが定数は大きい。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `insert`, `erase` | 償却 $O(P\log Q)$ | 一時領域 $O(P\log Q)$、全体 $O(S)$ |
| `pattern_count` | 期待 $O(P)$（文字列のハッシュ計算） | $O(1)$ |
| `count_matches` | 償却 $O(T\log Q)$ | $O(1)$ |

$S$ は正負の静的オートマトン群が保持する総文字数である。削除しても正の側から取り除かれるわけではないため、$S$ は削除後も減らない。

## 注意点

- 検索結果はパターンごとの出現回数ではなく、全登録パターンの出現回数の合計である。
- 文字は `unsigned char` として扱い、任意のバイト列を扱える。

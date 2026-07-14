---
title: Aho Corasick (Aho-Corasick法) [aho_corasick] [WPUZZLES]
documentation_of: ../src/algorithm/string/aho_corasick.hpp
---

複数パターンマッチング。

# テンプレート引数

```cpp
AhoCorasick<ALPHABET, MAX_NODES, OFFSET>
```

- 文字種類数 `ALPHABET`
- ノード数の上限 `MAX_NODES`
- 先頭文字 `OFFSET`

# add / build

```cpp
int add(string_view pattern)
void build()
void clear()
```

パターンを追加してから `build` する。

# query

```cpp
long long count_matches(string_view text)
int next_state(int state, char c)
int terminal_count(int state)
int output_count(int state)
int parent(int state)
int failure_link(int state)
int output_link(int state)
```

`count_matches` は text に含まれる全パターンの出現回数の総和を返す。

`output_count` はその状態で終わるパターン数を fail link 分も含めて返す。

`output_link` は failure link をたどった先で最初に現れる終端状態を返す。

## 時間計算量

- build: $O(\Sigma V)$
- query: $O(|S|)$

## API別の時間計算量・空間計算量

$V$ を使用node数、$P$ を追加pattern長、$T$ をtext長、$\Sigma=ALPHABET$ とする。

| API | 時間計算量 |
| --- | --- |
| `AhoCorasick()` | $O(MAX_NODES\cdot\Sigma)$（固定配列初期化） |
| `node_count()` / `is_built()` | $O(1)$（metadata query） |
| `clear()` | $O(V\Sigma)$ |
| `add(pattern)` | $O(P\Sigma)$ 最悪、既存経路だけなら $O(P)$ |
| `build()` | $O(V\Sigma)$（failure link前処理） |
| `next_state(state,c)` | $O(1)$ |
| `terminal_count(state)` / `output_count(state)` | $O(1)$ |
| `parent(state)` / `failure_link(state)` / `output_link(state)` | $O(1)$ |
| `count_matches(text)` | $O(T)$ |

`add` で新規nodeを作ると長さ $\Sigma$ の遷移表を初期化する。保持領域は $O(MAX_NODES\cdot\Sigma)$。`count_matches` は一致位置を列挙せず総数だけを返す。
## 注意点

- constructorはroot node 0だけを作る。`node_count()` は使用node数、
  `is_built()` はfailure link構築済みかを返す。
- `clear()` は使用中のnodeだけを初期化して未構築状態へ戻す。以前取得したstateは
  すべて無効になる。
- `add(pattern)` はpatternの終端node番号を返し、同じpatternの重複追加も数える。
  文字はalphabet内、node数は `MAX_NODES` 以下が必要。
  `build()` 後の追加は例外。
- `build()` はfailure linkと未定義遷移を構築する。pattern追加後に呼ぶ。
- `next_state(state,c)` は1文字読んだ次状態。
  `terminal_count(state)` はそのnodeで直接終わる登録pattern数、
  `output_count(state)` はfailure link上を含めて現在位置で終わるpattern数。
- `parent(state)` はtrie上の親nodeを返す。rootの親はroot自身。
  `failure_link(state)` は最長の真の接尾辞に対応するnodeを返し、
  `build()` 前は例外。
- `output_link(state)` はfailure link上で最も近い、直接patternが終端するstateを
  返す。該当stateがなければ `-1`。現在state自身は含めず、`build()` 前は
  例外。
- `count_matches(text)` は重複pattern・重なる出現・空patternを含む総出現数。
  遷移・output・検索はbuild前、またはstate範囲外なら例外。

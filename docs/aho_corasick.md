---
title: Aho Corasick
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
```

パターンを追加してから `build` する。

# query

```cpp
long long count_matches(string_view text)
int next_state(int state, char c)
int terminal_count(int state)
int output_count(int state)
```

`count_matches` は text に含まれる全パターンの出現回数の総和を返す。

`output_count` はその状態で終わるパターン数を fail link 分も含めて返す。

## 時間計算量

- build: $O(\Sigma V)$
- query: $O(|S|)$

## API別計算量

$V$ を使用node数、$P$ を追加pattern長、$T$ をtext長、$\Sigma=ALPHABET$ とする。

| API | 時間計算量 |
| --- | --- |
| `AhoCorasick()` | $O(MAX_NODES\cdot\Sigma)$（固定配列初期化） |
| `node_count()` / `is_built()` | $O(1)$（metadata query） |
| `add(pattern)` | $O(P\Sigma)$ 最悪、既存経路だけなら $O(P)$ |
| `build()` | $O(V\Sigma)$（failure link前処理） |
| `next_state(state,c)` | $O(1)$ |
| `terminal_count(state)` / `output_count(state)` | $O(1)$ |
| `count_matches(text)` | $O(T)$ |

`add` で新規nodeを作ると長さ $\Sigma$ の遷移表を初期化する。保持領域は $O(MAX_NODES\cdot\Sigma)$。`count_matches` は一致位置を列挙せず総数だけを返す。

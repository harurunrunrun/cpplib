---
title: Aho Corasick (Aho-Corasick法)
documentation_of: ../../../../src/algorithm/string/automata/aho_corasick.hpp
---

複数パターンの文字列照合を行う Aho--Corasick オートマトンである。

## テンプレート引数

```cpp
AhoCorasick<ALPHABET, MAX_NODES = -1, OFFSET = 'a'>
```

- `ALPHABET`: 文字の種類数（正）。文字 `c` は `c - OFFSET` が $[0,\texttt{ALPHABET})$ に入る必要がある。
- `MAX_NODES`: ノード数の上限（正）。省略時の `-1` は可変長版で、ノード追加に合わせて `std::vector` を伸ばす。
- `OFFSET`: 先頭の文字。

```cpp
AhoCorasick<26> dynamic;           // 可変長版
AhoCorasick<26, 100000> fixed;     // ノード数上限 100000 の固定長版
```

## 構築

```cpp
AhoCorasick()
int add(string_view pattern)
void build()
void clear()
int node_count() const
bool is_built() const
```

- コンストラクタは根ノード 0 だけを作る。
- `add(pattern)` はパターンをトライに追加し、終端ノードの番号を返す。同じパターンの重複追加も別々に数える。`build()` 後の追加は例外を送出する。
- `build()` は失敗リンクと未定義の遷移を構築する。パターンを追加した後に呼ぶ。構築済みなら何もしない。
- `clear()` は使用中のノードだけを初期化し、未構築状態へ戻す。以前に取得した状態番号はすべて無効になる。
- `node_count()` は使用ノード数、`is_built()` は `build()` 済みかどうかを返す。

## クエリ

```cpp
long long count_matches(string_view text) const
int next_state(int state, char c) const
int terminal_count(int state) const
int output_count(int state) const
int parent(int state) const
int failure_link(int state) const
int output_link(int state) const
```

- `count_matches(text)` は `text` に含まれる全パターンの出現回数の総和を返す。重複パターン・重なる出現・空パターンも数える。一致位置は列挙しない。
- `next_state(state, c)` は状態 `state` から 1 文字 `c` を読んだ次の状態を返す。
- `terminal_count(state)` はそのノードで直接終わる登録パターン数を返す。
- `output_count(state)` は失敗リンクをたどった先も含め、現在位置で終わるパターン数を返す。
- `parent(state)` はトライ上の親ノードを返す。根の親は根自身である。
- `failure_link(state)` は、その状態が表す文字列の最長の真の接尾辞に対応するノードを返す。
- `output_link(state)` は、失敗リンクをたどって最初に現れる、直接パターンが終わる状態を返す。現在の状態自身は含めない。該当する状態がなければ `-1` を返す。

## 時間計算量

$V$ を使用ノード数、$P$ を追加するパターンの長さ、$T$ をテキスト長、$\Sigma$ を `ALPHABET` とする。

| API | 時間計算量 |
| --- | --- |
| `AhoCorasick()` | 固定長版 $O(\texttt{MAX_NODES}\cdot\Sigma)$（固定配列の初期化）、可変長版 $O(\Sigma)$ |
| `node_count()` / `is_built()` | $O(1)$ |
| `clear()` | $O(V\Sigma)$ |
| `add(pattern)` | 最悪 $O(P\Sigma)$、既存の経路だけをたどるなら $O(P)$ |
| `build()` | $O(V\Sigma)$ |
| `next_state(state, c)` | $O(1)$ |
| `terminal_count(state)` / `output_count(state)` | $O(1)$ |
| `parent(state)` / `failure_link(state)` / `output_link(state)` | $O(1)$ |
| `count_matches(text)` | $O(T)$ |

`add` で新規ノードを作ると、長さ $\Sigma$ の遷移表を初期化する。可変長版ではノード追加時の再確保を償却して含む。

## 空間計算量

固定長版は $O(\texttt{MAX_NODES}\cdot\Sigma)$、可変長版は $O(V\Sigma)$。

## 注意点

- 文字が範囲外のとき、ノード数が `MAX_NODES` を超えるときは `std::runtime_error` を送出する。
- `next_state`、`output_count`、`failure_link`、`output_link`、`count_matches` は `build()` 前に呼ぶと `std::runtime_error` を送出する。
- 状態番号が範囲外のときも `std::runtime_error` を送出する。

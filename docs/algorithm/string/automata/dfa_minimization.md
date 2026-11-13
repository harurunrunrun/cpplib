---
title: DFA Minimization (Hopcroft) (DFA最小化)
documentation_of: ../../../../src/algorithm/string/automata/dfa_minimization.hpp
---

完全な決定性有限オートマトン（DFA）の状態の同値類と最小 DFA を、Hopcroft 法で求める。
遷移表の各行の添字をアルファベットの記号とみなし、受理状態は 0 以外の値で指定する。

## 入力の表現

~~~cpp
vector<vector<int>> transition;
vector<char> accepting;
~~~

`transition[state][symbol]` は遷移先である。
全状態で記号数が等しく、各遷移先が $[0,N)$ に入る完全 DFA を渡す。
不完全な DFA は、呼び出す前に非受理の吸収状態（sink）を追加して完全化しておく。

## 型

### `DfaEquivalenceClasses`

~~~cpp
struct DfaEquivalenceClasses {
    vector<int> state_to_class;
    vector<vector<int>> classes;
};
~~~

- `state_to_class[v]` は元の状態 `v` が属する同値類の番号である。
- `classes[c]` は同値類 `c` に属する元の状態を昇順に格納する。
- 同値類の番号は、各同値類に含まれる最小の元状態番号の昇順に振られる。

### `DfaMinimizationResult`

~~~cpp
struct DfaMinimizationResult {
    vector<vector<int>> transition;
    vector<char> accepting;
    int initial_state;
    vector<int> original_to_minimized;
    vector<vector<int>> minimized_to_original;
};
~~~

- `transition` と `accepting` は最小 DFA の遷移と受理フラグである。
- `initial_state` は最小 DFA の初期状態であり、正常な返り値では常に 0 である。
- `original_to_minimized[v]` は元の状態 `v` に対応する最小 DFA の状態である。初期状態から到達不能な元の状態では `-1` になる。
- `minimized_to_original[q]` は、最小 DFA の状態 `q` にまとめられた到達可能な元の状態を昇順に格納する。
- 最小 DFA の状態番号は、初期状態から記号番号の昇順に BFS した順に振られる。

## API

### `hopcroft_dfa_equivalence_classes`

~~~cpp
DfaEquivalenceClasses hopcroft_dfa_equivalence_classes(
    const vector<vector<int>>& transition,
    const vector<char>& accepting
);
~~~

到達可能性に関係なく、渡した全状態を右言語が等しい同値類へ分割する。
空の DFA（$N=0$）も受け取り、空の結果を返す。
元の初期状態を考慮した最小 DFA が必要なら `hopcroft_minimize_dfa` を使う。

### `hopcroft_minimize_dfa`

~~~cpp
DfaMinimizationResult hopcroft_minimize_dfa(
    const vector<vector<int>>& transition,
    const vector<char>& accepting,
    int initial_state
);
~~~

`initial_state` から到達不能な状態を除き、到達可能な部分を Myhill--Nerode 同値で割った最小の完全 DFA を返す。
返り値は元の DFA と同じ言語を受理する。この API には空の DFA を渡せない。

## 時間計算量・空間計算量

$N$ を入力の状態数、$A$ をアルファベットのサイズ、
$R$ を初期状態から到達可能な状態数、$M$ を最小 DFA の状態数とする。
逆遷移と返り値の領域も含めて示す。

| API / 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 入力検証 | $O(NA)$ | $O(1)$ |
| `hopcroft_dfa_equivalence_classes` | $O(N+AN\log N)$ | $O(N+AN)$ |
| `hopcroft_minimize_dfa` の到達可能性探索 | $O(RA)$ | $O(N)$ |
| `hopcroft_minimize_dfa` の分割・商 DFA 構築を含む全体 | $O(NA+R+AR\log R)$ | $O(N+AR+MA)$ |
| `state_to_class[v]` / `original_to_minimized[v]` | $O(1)$ | 追加 $O(1)$ |
| `classes[c]` / `minimized_to_original[q]` の全列挙 | 列挙する要素数に比例 | 追加 $O(1)$ |

$A=0$ では分割処理・最小化ともに $O(N)$ になる。

## 注意点

- `transition` と `accepting` の大きさが異なる、遷移表の行の長さが揃っていない、遷移先が範囲外、という入力では、両 API とも `std::runtime_error` を送出する。
- `hopcroft_minimize_dfa` では `initial_state` が範囲外の場合も `std::runtime_error` を送出する。
- 状態番号・記号番号はすべて 0-indexed である。

---
title: Monotone Queue (単調キュー) [DSL_3_D]
documentation_of: ../../../../src/structure/range_query/extrema/monotone_queue.hpp
---

単調な両端キューである。添字を単調増加の順に追加し、期限切れの要素を先頭から取り除く。

## キュー

~~~cpp
template<class T, int MAX_SIZE = -1, class Compare = std::less<T>>
struct MonotoneQueue;

template<class T, int MAX_SIZE = -1>
using MonotoneMinQueue = MonotoneQueue<T, MAX_SIZE, std::less<T>>;

template<class T, int MAX_SIZE = -1>
using MonotoneMaxQueue = MonotoneQueue<T, MAX_SIZE, std::greater<T>>;
~~~

`Compare` の意味で先に来る値を `best()` とする。同じ値を追加した場合は、新しい添字の要素だけを残す。`MAX_SIZE` は同時に保持する候補数の上限であり、省略時（`-1`）は上限を設けず、必要に応じて `std::vector` で領域を拡張する。

~~~cpp
explicit MonotoneQueue(const Compare& compare = Compare())
int size() const
bool empty() const
void clear()
void push(long long index, const T& value)
void pop_front()
void expire(long long first_valid_index)
const T& best() const
long long best_index() const
~~~

- `push(index, value)` は添字 `index` の値 `value` を追加する。`index` はそれまでに追加した添字より大きくなければならない。
- `pop_front` は先頭（最良）の候補を取り除く。
- `expire(first_valid_index)` は添字が `first_valid_index` 未満の候補を先頭から取り除く。
- `best` は `Compare` の意味で最良の値への参照を、`best_index` はその入力時の添字を返す。
- `clear` はすべての候補と添字の単調性の記録を消去する。

## スライディングウィンドウ

~~~cpp
template<class T, int MAX_WINDOW = -1, class Compare = std::less<T>>
vector<T> sliding_window_extreme(const vector<T>& values, int window, const Compare& compare)

template<class T, int MAX_WINDOW = -1>
vector<T> sliding_window_min(const vector<T>& values, int window)
~~~

長さ `window` の各連続部分列について、`Compare` の意味での最良値を左から順に返す。

## 時間計算量

$N$ を入力列の長さ、$K$ をウィンドウ幅とし、比較を $O(1)$ とする。

- コンストラクタ: 固定長配列の初期化を含めて $O(\texttt{MAX_SIZE})$
- `size`, `empty`, `clear`, `pop_front`, `best`, `best_index`: $O(1)$
- `expire`: $R$ 個を期限切れとして取り除くとき $O(R+1)$。スライディングウィンドウの処理全体ではならし $O(1)$
- `push`: 1 回ではキューの大きさに線形だが、各候補は高々 1 回しか取り除かれないので、追加列全体でならし $O(1)$
- 各スライディングウィンドウ関数: 戻り値の $N-K+1$ 個の構築を含めて $O(N)$

## 空間計算量

- キュー: $O(\texttt{MAX_SIZE})$（省略時は同時に保持した候補数の最大値に比例）
- スライディングウィンドウ関数: キューと戻り値を含めて $O(\texttt{MAX_WINDOW}+N-K+1)$（省略時は $O(K+N-K+1)$）

## 注意点

- `push(index, value)` の `index` は追加列全体で狭義単調増加でなければならず、違反時は `std::runtime_error` を送出する。
- 候補数が `MAX_SIZE` に達した状態で、既存の候補に支配されない値を追加すると `std::runtime_error` を送出する。
- 空のキューへの `pop_front`, `best`, `best_index` は `std::runtime_error` を送出する。`expire` は空でも呼べる。
- スライディングウィンドウ関数は `1 <= window`、`window <= MAX_WINDOW`（指定時）、`window <= values.size()` を要求し、違反時は `std::runtime_error` を送出する。
- コピーはできず、ムーブはできる。
- 容量を省略した版で `T = bool` の場合、`best()` は `bool` を値で返す。他の型では const 参照を返すが、その参照は後続の `push` による再確保で無効になることがある。

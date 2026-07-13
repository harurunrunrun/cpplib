---
title: Monotone Queue
documentation_of: ../src/structure/other/monotone_queue.hpp
---

単調deque。添字を単調増加で追加し、期限切れ要素を先頭から除去する。

# queue

~~~cpp
MonotoneQueue<T, MAX_SIZE, Compare>
MonotoneMinQueue<T, MAX_SIZE>
MonotoneMaxQueue<T, MAX_SIZE>
~~~

`Compare` で先に来る値を `best()` とする。同値な値を追加した場合は新しい添字だけを残す。

~~~cpp
int size() const
bool empty() const
void clear()
void push(long long index, const T& value)
void pop_front()
void expire(long long first_valid_index)
const T& best() const
long long best_index() const
~~~

`push` の添字は、それまでに追加した添字より大きくなければならない。

# sliding window

~~~cpp
vector<T> sliding_window_extreme<T, MAX_WINDOW>(values, window, compare)
vector<T> sliding_window_min<T, MAX_WINDOW>(values, window)
vector<T> sliding_window_max<T, MAX_WINDOW>(values, window)
~~~

長さ `window` の各連続部分列に対する最小値または最大値を、左から順に返す。

# 計算量

$N$ を入力列長、$K$ をwindow幅とする。比較を $O(1)$ とする。

- constructor: 固定長arrayの初期化を含めて $O(\mathrm{MAX\_SIZE})$
- `size`, `empty`, `clear`, `pop_front`, `best`, `best_index`: $O(1)$
- `expire`: $R$ 個を期限切れとして除くと $O(R+1)$。sliding window処理全体では償却 $O(1)$
- `push`: 1回ではqueue sizeに対して線形、追加列全体では各候補を高々1回除去するため償却 $O(1)$
- 各 sliding window関数: 戻り値 $N-K+1$ 個の構築を含めて $O(N)$

# 空間計算量

- queue: $O(\mathrm{MAX\_SIZE})$
- sliding window関数: queueと戻り値を含めて $O(\mathrm{MAX\_WINDOW}+N-K+1)$

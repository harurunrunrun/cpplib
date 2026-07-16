---
title: Time Segment Tree (Offline Interval Events) (時間セグメント木)
documentation_of: ../src/algorithm/other/time_segment_tree.hpp
---

時刻区間 $[l,r)$ で有効なイベントを、rollback できる状態へ適用するオフライン時間軸 segment tree。
各時刻を独立に作り直さず、segment tree の DFS と rollback によって処理する。

## テンプレート引数

```cpp
TimeSegmentTree<Event, MAX_TIME, MAX_EVENTS>
OfflineIntervalEvents<Event, MAX_TIME, MAX_EVENTS>
```

- `Event`: 区間に載せるイベント型
- `MAX_TIME`: 時刻数の上限
- `MAX_EVENTS`: 空でない区間イベント数の上限

`OfflineIntervalEvents` は同じ型の別名。

## コンストラクタ

```cpp
explicit TimeSegmentTree(int time_count = MAX_TIME)
```

時刻 $0,1,\ldots,\mathtt{time\_count}-1$ を扱う。

## 時間計算量

$O(\mathtt{MAX\_TIME}+\mathtt{MAX\_EVENTS})$。

範囲外の `time_count` は固定長空間計算量を確保する前に検査する。

## 区間イベントの追加

```cpp
void add_interval(int left, int right, const Event& event)
void add_interval(int left, int right, Event&& event)

template<class... Args>
void emplace_interval(int left, int right, Args&&... args)
```

イベントを時刻区間 $[\mathtt{left},\mathtt{right})$ で有効にする。
`emplace_interval` は空間計算量上で `Event` を直接構築する。空区間は何も保存しない。

## 時間計算量

1 回につき $O(\log(\mathtt{MAX\_TIME}+1))$。このほかに `Event` のコピー、move、または構築の計算量がかかる。

## 実行

```cpp
events.run(
    [&](const Event& event){ state.apply(event); },
    [&](){ return state.snapshot(); },
    [&](auto snapshot){ state.rollback(snapshot); },
    [&](int time){ answer[time] = state.get(); }
);
```

次の callback を用いて時刻順に処理する。

- `apply(const Event&)`: 現在の状態へイベントを適用する
- `snapshot()`: 現在の rollback 位置を返す
- `rollback(snapshot)`: 状態を指定した位置へ戻す
- `answer(int time)`: 時刻 `time` の状態を処理する

`snapshot` の返り値は `rollback` に渡せる型でなければならない。`run` の終了時には、状態は呼出し開始時まで戻る。同じイベント集合に対する `run` は再実行できる。

同じ時刻で有効なイベントの適用順は追加順とは限らない。答えが有効イベントの集合だけで決まる場合、またはイベントの適用順に依存しない状態に用いる。

## 時間計算量

空でない区間イベント数を $M$、時刻数を $Q$、保存されたイベント参照数を $P=\mathtt{reference\_count()}$、
イベントを 1 個以上持つ segment tree node 数を $B$ とする。
イベント 1 個の `apply`、`snapshot` 1 回、`rollback` 1 回の計算量をそれぞれ $A,S,R$、時刻 $t$ の `answer` の計算量を $C_t$ とする。

$$
O\left(
Q+\log(\mathtt{MAX\_TIME}+1)
+P\cdot A+B\cdot(S+R)
+\sum_{t=0}^{Q-1} C_t
\right)
$$

ここで $P=O(M\log(\mathtt{MAX\_TIME}+1))$、$B\le P$。
`snapshot` と `rollback` はイベントを持つ node だけで各 1 回呼ばれ、
イベントを持たない node では呼ばれない。
`rollback` の計算量が取り消すイベント数に比例する場合、全 node を通した
取り消し回数は $P$ 回である。

## 初期化

```cpp
void clear()
```

登録中のイベント数を $M$ として、登録したイベントをすべて消す。`time_count` は変えない。

## 時間計算量

$O(\mathtt{MAX\_TIME}+M)$。

## API

```cpp
int size() const
int event_count() const
int reference_count() const
static constexpr int event_capacity()
static constexpr int time_capacity()
static constexpr std::size_t reference_capacity()
```

- `size`: 実際の時刻数
- `event_count`: 保存中の空でない区間イベント数
- `reference_count`: segment tree node に載せたイベント参照数
- `event_capacity`, `time_capacity`, `reference_capacity`: 各固定容量

すべて $O(1)$。

## 注意点

- `0 <= time_count <= MAX_TIME`。違反時は例外。
- 追加区間は `0 <= left <= right <= time_count`。違反時は例外。
- 空でない区間を `MAX_EVENTS` 個より多く追加すると例外。
- 各イベントは 1 個だけ保持し、segment tree node には参照を固定長 `std::array` へ保存する。
- callback から例外を送出した場合、処理中の snapshot まで rollback することは保証しない。

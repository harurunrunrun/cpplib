---
title: Time Segment Tree (Offline Interval Events) (時間セグメント木)
documentation_of: ../../../../../src/algorithm/other/offline_query/batch_framework/time_segment_tree.hpp
---

時刻区間 $[l,r)$ で有効なイベントを、rollback できる状態へ適用するオフライン用の時間軸セグメント木。各時刻の状態を独立に作り直さず、セグメント木上の DFS と rollback によって処理する。

## テンプレート引数

```cpp
TimeSegmentTree<Event, MAX_TIME = -1, MAX_EVENTS = -1>
OfflineIntervalEvents<Event, MAX_TIME = -1, MAX_EVENTS = -1>
```

- `Event`: 区間に載せるイベントの型
- `MAX_TIME`: 時刻数の上限
- `MAX_EVENTS`: 空でない区間イベント数の上限

`OfflineIntervalEvents` は同じ型の別名である。

容量テンプレート引数を省略する（どちらかが負になる）と、実行時サイズに合わせて伸びる `std::vector` を内部で使う可変長版になる。容量を明示すると固定長版になり、内部配列は構築時に一括確保される。

```cpp
TimeSegmentTree<int> dynamic(time_count);
TimeSegmentTree<int, 100000, 100000> fixed(time_count);
```

コピーはできず、ムーブはできる。

## コンストラクタ

```cpp
explicit TimeSegmentTree(int time_count = MAX_TIME)  // 可変長版のデフォルトは 0
```

時刻 $0,1,\ldots,T-1$（$T$ は `time_count`）を扱う。`time_count` の範囲は固定長の領域を確保する前に検査する。

### 時間計算量

固定長版は $O(\texttt{MAX_TIME}+\texttt{MAX_EVENTS})$、可変長版は $O(T)$。

## 区間イベントの追加

```cpp
void add_interval(int left, int right, const Event& event)
void add_interval(int left, int right, Event&& event)

template<class... Args>
void emplace_interval(int left, int right, Args&&... args)
```

イベントを時刻区間 $[\mathtt{left},\mathtt{right})$ で有効にする。`emplace_interval` は内部の領域上で `Event` を直接構築する。空区間では何も保存しない。

### 時間計算量

1 回につき、固定長版は $O(\log(\texttt{MAX_TIME}+1))$、可変長版は償却 $O(\log T)$。このほかに `Event` のコピー、ムーブ、または構築の計算量がかかる。

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

`snapshot` の返り値は `rollback` に渡せる型でなければならない。`run` の終了時には、状態は呼出し開始時まで戻る。同じイベント集合に対して `run` を何度でも実行できる。`time_count == 0` のときは何も呼ばない。

同じ時刻で有効なイベントの適用順は追加順とは限らない。答えが有効イベントの集合だけで決まる場合、またはイベントの適用順に依存しない状態に用いること。

### 時間計算量

空でない区間イベント数を $M$、時刻数を $Q$、保存されたイベント参照数（`reference_count()`）を $P$、イベントを 1 個以上持つセグメント木のノード数を $B$ とする。イベント 1 個の `apply`、`snapshot` 1 回、`rollback` 1 回の計算量をそれぞれ $A,S,R$、時刻 $t$ の `answer` の計算量を $C_t$ とすると、全体は次のとおり（$W$ はセグメント木の葉数で、固定長版では $\texttt{MAX_TIME}$ 以上の最小の 2 冪、可変長版では $T$ 以上の最小の 2 冪）。

$$
O\left(
Q+\log W
+P\cdot A+B\cdot(S+R)
+\sum_{t=0}^{Q-1} C_t
\right)
$$

ここで $P=O(M\log W)$、$B\le P$ である。`snapshot` と `rollback` はイベントを持つノードでだけ各 1 回呼ばれ、イベントを持たないノードでは呼ばれない。`rollback` の計算量が取り消すイベント数に比例する場合、全ノードを通した取り消し回数は $P$ 回である。

## 初期化

```cpp
void clear()
```

登録したイベントをすべて消す。`time_count` は変えない。

### 時間計算量

登録中のイベント数を $M$ として $O(W+M)$。

## その他の API

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
- `reference_count`: セグメント木のノードに載せたイベント参照数
- `event_capacity`, `time_capacity`, `reference_capacity`: 各固定容量（可変長版では意味を持たない）

すべて $O(1)$。

## 空間計算量

固定長版は $O(\texttt{MAX_TIME}+\texttt{MAX_EVENTS}\log\texttt{MAX_TIME})$、可変長版は登録区間数を $E$ として $O(T+E\log T)$。

## 注意点

- `0 <= time_count` かつ、固定長版では `time_count <= MAX_TIME` でなければならない。違反時は例外を送出する。
- 追加区間は `0 <= left <= right <= time_count` でなければならない。違反時は例外を送出する。
- 固定長版で空でない区間を `MAX_EVENTS` 個より多く追加すると例外を送出する。
- 各イベントは 1 個だけ保持し、セグメント木のノードにはその参照を保存する。
- callback から例外を送出した場合、処理中の snapshot まで rollback することは保証しない。

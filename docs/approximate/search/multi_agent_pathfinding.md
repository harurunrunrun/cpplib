---
title: Real-Time Multi-Agent Grid Pathfinding (実時間マルチエージェント格子経路探索)
documentation_of: ../../../src/approximate/search/multi_agent_pathfinding.hpp
---

4 近傍の単位コスト格子に対する HPA*、SIPP、予約表による協調探索、および CBS 系探索を提供する。すべての時刻は非負整数で、1 ステップは「上下左右への移動」または明記された待機である。

## `GridLocation`, `GridLocationHash`

```cpp
struct GridLocation { int row, column; };
struct GridLocationHash {
    size_t operator()(GridLocation location) const noexcept;
};
```

`GridLocation` は値比較と辞書順比較を持つ座標、`GridLocationHash` はハッシュコンテナ用関数オブジェクトである。比較とハッシュは $O(1)$。

## `MultiAgentGridMap`

```cpp
MultiAgentGridMap(int rows, int columns,
        const vector<GridLocation>& blocked = {});
int rows() const noexcept;
int columns() const noexcept;
int size() const noexcept;
bool contains(GridLocation p) const noexcept;
bool passable(GridLocation p) const noexcept;
void set_blocked(GridLocation p, bool blocked = true);
int index(GridLocation p) const;
GridLocation location(int index) const;
vector<GridLocation> neighbors(GridLocation p) const;
```

固定長ビット列で障害物を保持する4近傍格子。構築は格子セル数を $V$、初期障害数を $B$ として $O(V+B)$ 時間・$O(V)$ 空間、各照会・更新は $O(1)$、`neighbors` は $O(1)$ 時間・最大4要素。

## `TimedGridLocation`, `TimedGridPathResult`

```cpp
struct TimedGridLocation { GridLocation position; int time; };
struct TimedGridPathResult {
    vector<TimedGridLocation> path;
    int cost;
    size_t expanded;
    explicit operator bool() const noexcept;
};
```

時刻付き経路と単一エージェント探索結果。未発見時は `cost == -1`、成功時の `cost` は到着時刻と開始時刻の差である。真偽変換は $O(1)$。

## `ReservationTable`

```cpp
void reserve_vertex(GridLocation p, int time);
void reserve_edge(GridLocation from, GridLocation to, int time);
void reserve_path(const vector<TimedGridLocation>& path,
                  int hold_until = -1);
bool is_vertex_reserved(GridLocation p, int time) const;
bool is_edge_reserved(GridLocation from, GridLocation to, int time) const;
vector<pair<int,int>> safe_intervals(
    GridLocation p, int first_time, int last_time) const;
int maximum_time() const noexcept;
size_t reserved_vertex_count() const noexcept;
size_t reserved_edge_count() const noexcept;
```

頂点占有と時刻 `time` から `time+1` の有向辺移動を格納する。頂点予約・照会は同一セルの予約数を $R_p$ として期待 $O(\log R_p)$、辺予約・照会は期待 $O(1)$。`reserve_path` は経路長と保持時間の合計を $L$ として期待 $O(L\log R_p)$。`safe_intervals` は範囲内予約数を $K$ として期待 $O(\log R_p+K)$ 時間・$O(K)$ 出力空間。個数照会のうち頂点総数だけは $O(P)$（予約されたセル数 $P$）、その他は $O(1)$。

## `safe_interval_path_planning`, `sipp_search`

```cpp
safe_interval_path_planning(map, start, goal, reservations,
                            start_time, max_time,
                            goal_hold_until = -1);
sipp_search(map, start, goal, reservations,
            start_time, max_time, goal_hold_until = -1);
```

同一実装を指す正式名と略称。セルごとの安全区間を状態とし、待機をまとめて遷移する Safe Interval Path Planning。逆向き予約辺も検査するため正面交換衝突を避ける。生成した安全区間状態数を $S$、遷移候補を $A$、優先度キュー最大量を $Q$、参照した予約数を $R$ とすると $O((S+A)\log Q+R)$ 時間、$O(S+Q+L)$ 空間。

## `hpa_star_search`

```cpp
hpa_star_search(map, start, goal, int cluster_size = 8);
```

格子を正方クラスタへ分け、クラスタ境界の通行可能セルを入口として抽象グラフを作り、各クラスタ内の正確な BFS 距離と境界横断辺を A* で結ぶ HPA*。入口数を $P$、クラスタ最大セル数を $C$、抽象辺数を $E_H$ とすると前処理込みで $O(V+PC+P^2+(P+E_H)\log P)$ 時間、$O(V+PC+E_H)$ 空間。返す経路は元の格子上で連続する。

## `MultiAgentPathResult`

```cpp
struct MultiAgentPathResult {
    vector<vector<GridLocation>> paths;
    int sum_of_costs, makespan;
    size_t expanded;
    bool cutoff;
    explicit operator bool() const noexcept;
};
```

各エージェントの時刻0からの経路、到着コスト和、最大到着時刻、展開量、打ち切り状態を返す。未発見時は `sum_of_costs == -1`。真偽変換は $O(1)$。

## `cooperative_a_star_search`

```cpp
cooperative_a_star_search(map, starts, goals, max_time,
                          initial_reservations = {});
```

固定優先順位で各エージェントを SIPP により順に計画し、得た経路とゴール待機を予約する Cooperative A*。各低レベル探索の状態・遷移数を $S_i,A_i$ とすると、時間は $\sum_i O((S_i+A_i)\log S_i)$、予約表と全経路を含む空間は $O(\sum_i(S_i+L_i))$。完全探索ではなく、優先順位によっては解が存在しても失敗する。

## `windowed_hierarchical_cooperative_a_star_search`, `whca_star_search`

```cpp
windowed_hierarchical_cooperative_a_star_search(
    map, starts, goals, window, max_steps, initial_reservations = {});
whca_star_search(map, starts, goals, window, max_steps,
                 initial_reservations = {});
```

正式名と略称は同一実装。長さ `window` の時空間 A* を固定優先順位で再計画し、先頭1手だけを実行する Windowed Hierarchical Cooperative A*。1回の窓内状態数を $S_w$、エージェント数を $k$ とすると最悪 $O(\text{max_steps}\,k S_w\log S_w)$ 時間、$O(kS_w+\sum L_i)$ 空間。制限内に全員が到着しなければ `cutoff` が真となる。

## `bounded_conflict_based_search`

```cpp
bounded_conflict_based_search(
    map, starts, goals, max_high_level_nodes, max_time);
```

最小コストの衝突木ノードを展開し、最初の頂点衝突または辺交換衝突を二分する打ち切り付き CBS。上限に達して OPEN が残れば `cutoff` が真。高レベル展開数を $H$、1回の時空間 A* 状態数を $S$ とすると $O(H(kL^2+S\log S))$ 時間、$O(H(kL+S))$ 空間が上界の目安である。打ち切られず成功した解はコスト和最適。

## `enhanced_conflict_based_search`, `enhanced_cbs_search`

```cpp
enhanced_conflict_based_search(
    map, starts, goals, max_high_level_nodes, max_time);
enhanced_cbs_search(
    map, starts, goals, max_high_level_nodes, max_time);
```

正式名と略称は同一実装。各候補衝突の両制約を試して cardinal / semi-cardinal / non-cardinal を判定し、より強い衝突を優先する。また同コストで衝突数を減らす子への bypass を行う Enhanced CBS。衝突候補数を $F$ とすると、分類により高レベル1展開当たり最大 $O(FS\log S)$ が加わる。空間量と最適性は打ち切り付き CBS と同じ。

## `ecbs_search`

```cpp
ecbs_search(map, starts, goals, weight,
            max_high_level_nodes, max_time);
```

OPEN の最小コストの `weight` 倍以内を FOCAL とし、その中から衝突数最小のノードを選ぶ Enhanced CBS。`weight >= 1` が必要。低レベルは最適 A* なので、許された時刻範囲内で成功した解のコスト和は最適値の `weight` 倍以下。高レベル OPEN サイズを $Q_H$ とすると、FOCAL 選択の走査を含め $O(H(Q_H+kL^2+S\log S))$ 時間、$O(H(kL+S))$ 空間。

## 時間計算量

各公開操作の時間計算量と空間計算量は上の各 API 節に記載した。単一エージェント法は生成した時空間状態に対してほぼ線形対数、CBS 系は衝突木展開数に比例し、利用者指定の上限で停止する。

## 注意点

- 格子は探索中に変更しない。開始・目標は通行可能、開始位置は互いに異なる必要がある。不正値は `std::invalid_argument` または `std::out_of_range`。
- Cooperative A* / WHCA* は高速な不完全法、CBS系は制限付き完全法であり、用途と失敗理由が異なる。
- CBS系では経路末尾のエージェントはゴールに待機し続けるものとして衝突判定する。`max_time` が小さすぎる場合は解なしと区別できないため、十分な上限を与える。
- ECBS の品質保証は単位非負コスト、正確な低レベル探索、および打ち切り前の成功を仮定する。

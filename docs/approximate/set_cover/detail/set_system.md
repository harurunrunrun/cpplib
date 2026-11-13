---
title: Set System Helpers (集合系補助)
documentation_of: ../../../../src/approximate/set_cover/detail/set_system.hpp
---

集合被覆のヒューリスティックが共有する、集合系の正規化と増分（gain）更新のための内部 API である。宇宙の大きさを $U$ とする。

## API

### `SetSystem`

正規化済み集合列 `sets` と、各要素を含む集合添字列 `incidence` を保持する。総出現数を $L$ として空間計算量は $O(U+L)$。

### `normalize_set_system(universe_size, input)`

入力をコピーして各集合を昇順に並べて重複を除き、各要素を含む集合の一覧（`incidence`）を構築する。入力集合の長さを $l_i$ として時間計算量は $O(U+\sum_i l_i\log l_i)$、追加空間計算量は $O(U+L)$。

### `GainEntry`, `GainEntryLess`

`GainEntry` は増分（未被覆要素数）`gain` と集合添字 `index` を保持する。`GainEntryLess` は `std::priority_queue` で gain の大きい順、同値なら添字の小さい順に取り出されるような比較である。比較の時間・追加空間計算量は $O(1)$。

### `cover_set(selected, system, covered, gains, on_gain_changed)`

集合 `selected` で新たに覆われる要素に印を付け、その要素を含む全集合の `gains` を 1 減らして `on_gain_changed(set_index, new_gain)` を呼ぶ。時間計算量は走査した `incidence` の要素数と、コールバックの実行時間の総和に比例する。追加空間計算量は $O(1)$。

### `collect_elements(covered, desired_state)`

指定状態の要素添字を昇順で返す。時間・追加空間計算量は $O(U)$。

## 注意点

- 名前空間 `approximate::set_cover::detail` の内部 API である。
- `normalize_set_system` は範囲外の要素に対して `std::out_of_range` を投げる。
- `cover_set` は配列の大きさと `selected` の妥当性を検査しない。

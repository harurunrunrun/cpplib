---
title: Set System Helpers (集合系補助)
documentation_of: ../src/approximate/set_cover/detail/set_system.hpp
---

## Set System Helpers (集合系補助)

set cover heuristicが共有する集合系正規化とgain更新の内部API。

## API

### `SetSystem`

正規化済み集合列 `sets` と、各要素を含む集合添字列 `incidence` を保持する。総出現数を $L$ として空間計算量は $O(U+L)$。

### `normalize_set_system(universe_size, input)`

各集合を昇順・一意にし、incidence listを構築する。入力集合長を $l_i$ として時間計算量は $O(\sum_i l_i\log l_i)$、追加空間計算量は $O(U+L)$。

### `GainEntry`, `GainEntryLess`

未被覆要素数と集合添字を保持し、gain降順・同値なら添字昇順となるpriority比較を提供する。比較の時間・追加空間計算量は $O(1)$。

### `cover_set(selected, system, covered, gains, on_gain_changed)`

選択集合で新たに覆われる要素をmarkし、関連する全集合のgainを減らしてcallbackする。走査したincidence数とcallback時間の総和に比例し、追加空間計算量は $O(1)$。

### `collect_elements(covered, desired_state)`

指定状態の要素添字を昇順で返す。時間・追加空間計算量は $O(U)$。

## 注意点

`approximate::set_cover::detail` の内部APIである。範囲外要素では `std::out_of_range` を送出する。`cover_set` は配列sizeと `selected` の妥当性を検査しない。

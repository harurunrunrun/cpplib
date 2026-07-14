#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from collections import Counter, defaultdict
from pathlib import Path


DIRECTIONS = (
    (-1, 0, "A"),
    (-1, 1, "B"),
    (0, 1, "C"),
    (1, 1, "D"),
    (1, 0, "E"),
    (1, -1, "F"),
    (0, -1, "G"),
    (-1, -1, "H"),
)


Placement = tuple[int, int, str]
Case = tuple[list[str], list[str], list[Placement]]


def inside(grid: list[str], row: int, column: int) -> bool:
    return 0 <= row < len(grid) and 0 <= column < len(grid[0])


def find_occurrences(grid: list[str], word: str) -> list[Placement]:
    result: list[Placement] = []
    for row in range(len(grid)):
        for column in range(len(grid[0])):
            for dr, dc, direction in DIRECTIONS:
                end_row = row + (len(word) - 1) * dr
                end_column = column + (len(word) - 1) * dc
                if not inside(grid, end_row, end_column):
                    continue
                if all(
                    grid[row + offset * dr][column + offset * dc] == character
                    for offset, character in enumerate(word)
                ):
                    result.append((row, column, direction))
    return result


def solve_small_case(grid: list[str], words: list[str]) -> list[Placement]:
    result: list[Placement] = []
    for word in words:
        occurrences = find_occurrences(grid, word)
        assert len(occurrences) == 1, (word, occurrences)
        result.append(occurrences[0])
    return result


def unique_candidates(grid: list[str], minimum_length: int = 2) -> dict[str, Placement]:
    occurrences: dict[str, list[Placement]] = defaultdict(list)
    for row in range(len(grid)):
        for column in range(len(grid[0])):
            for dr, dc, direction in DIRECTIONS:
                characters: list[str] = []
                current_row = row
                current_column = column
                while inside(grid, current_row, current_column):
                    characters.append(grid[current_row][current_column])
                    if len(characters) >= minimum_length:
                        occurrences["".join(characters)].append(
                            (row, column, direction)
                        )
                    current_row += dr
                    current_column += dc
    return {
        word: positions[0]
        for word, positions in occurrences.items()
        if len(positions) == 1
    }


def random_grid(rng: random.Random, rows: int, columns: int) -> list[str]:
    return [
        "".join(rng.choice(string.ascii_uppercase) for _ in range(columns))
        for _ in range(rows)
    ]


def make_direction_case(rng: random.Random) -> Case:
    while True:
        grid = random_grid(rng, 8, 9)
        candidates = unique_candidates(grid, minimum_length=4)
        words: list[str] = []
        for _, _, direction in DIRECTIONS:
            choices = [
                word
                for word, placement in candidates.items()
                if placement[2] == direction and word not in words
            ]
            if not choices:
                break
            words.append(choices[0])
        if len(words) == 8:
            return grid, words, solve_small_case(grid, words)


def make_random_case(rng: random.Random, rows: int, columns: int) -> Case:
    while True:
        grid = random_grid(rng, rows, columns)
        candidates = list(unique_candidates(grid, minimum_length=3))
        if len(candidates) >= 20:
            rng.shuffle(candidates)
            words = candidates[: rng.randint(1, min(20, len(candidates)))]
            return grid, words, solve_small_case(grid, words)


def write_suite(out_dir: Path, stem: str, cases: list[Case]) -> None:
    assert 1 <= len(cases) <= 10
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case_index, (grid, words, placements) in enumerate(cases):
        assert 1 <= len(grid) <= 1000
        assert 1 <= len(grid[0]) <= 1000
        assert 1 <= len(words) <= 1000
        input_lines.append(f"{len(grid)} {len(grid[0])} {len(words)}")
        input_lines.extend(grid)
        input_lines.extend(words)
        output_lines.extend(
            f"{row} {column} {direction}"
            for row, column, direction in placements
        )
        if case_index + 1 != len(cases):
            output_lines.append("")
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def official_sample() -> Case:
    grid = [
        "QWSPILAATIRAGRAMYKEI",
        "AGTRCLQAXLPOIJLFVBUQ",
        "TQTKAZXVMRWALEMAPKCW",
        "LIEACNKAZXKPOTPIZCEO",
        "FGKLSTCBTROPICALBLBC",
        "JEWHJEEWSMLPOEKORORA",
        "LUPQWRNJOAAGJKMUSJAE",
        "KRQEIOLOAOQPRTVILCBZ",
        "QOPUCAJSPPOUTMTSLPSF",
        "LPOUYTRFGMMLKIUISXSW",
        "WAHCPOIYTGAKLMNAHBVA",
        "EIAKHPLBGSMCLOGNGJML",
        "LDTIKENVCSWQAZUAOEAL",
        "HOPLPGEJKMNUTIIORMNC",
        "LOIUFTGSQACAXMOPBEIO",
        "QOASDHOPEPNBUYUYOBXB",
        "IONIAELOJHSWASMOUTRK",
        "HPOIYTJPLNAQWDRIBITG",
        "LPOINUYMRTEMPTMLMNBO",
        "PAFCOPLHAVAIANALBPFS",
    ]
    words = [
        "MARGARITA",
        "ALEMA",
        "BARBECUE",
        "TROPICAL",
        "SUPREMA",
        "LOUISIANA",
        "CHEESEHAM",
        "EUROPA",
        "HAVAIANA",
        "CAMPONESA",
    ]
    placements = solve_small_case(grid, words)
    assert placements == [
        (0, 15, "G"),
        (2, 11, "C"),
        (7, 18, "A"),
        (4, 8, "C"),
        (16, 13, "B"),
        (4, 15, "E"),
        (10, 3, "D"),
        (5, 1, "E"),
        (19, 7, "C"),
        (11, 11, "H"),
    ]
    return grid, words, placements


def maximum_case() -> Case:
    size = 1000
    rng = random.Random(413086)
    grid = random_grid(rng, size, size)
    words = list(grid)

    columns = ["".join(grid[row][column] for row in range(size)) for column in range(size)]
    main_diagonal = "".join(grid[index][index] for index in range(size))
    anti_diagonal = "".join(grid[index][size - 1 - index] for index in range(size))
    full_length_lines = (
        grid
        + [row[::-1] for row in grid]
        + columns
        + [column[::-1] for column in columns]
        + [main_diagonal, main_diagonal[::-1], anti_diagonal, anti_diagonal[::-1]]
    )
    counts = Counter(full_length_lines)
    assert all(counts[word] == 1 for word in words)
    placements = [(row, 0, "C") for row in range(size)]
    return grid, words, placements


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260714)
    write_suite(out_dir, "official_sample", [official_sample()])
    write_suite(out_dir, "all_directions", [make_direction_case(rng)])
    write_suite(
        out_dir,
        "maximum_test_count",
        [make_random_case(rng, 3 + index % 5, 4 + index % 4) for index in range(10)],
    )
    write_suite(out_dir, "maximum_grid_words_and_length", [maximum_case()])


if __name__ == "__main__":
    main()

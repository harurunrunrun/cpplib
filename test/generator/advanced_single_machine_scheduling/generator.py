#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import heapq
import math
import random
from pathlib import Path

Case = tuple[list[int], list[int], list[int], list[int], list[int], list[list[int]]]


def srpt(processing: list[int], release: list[int]) -> tuple[int, list[tuple[int, int, int]]]:
    size = len(processing)
    arrivals = sorted(range(size), key=lambda job: (release[job], job))
    remaining = processing.copy()
    queue: list[tuple[int, int]] = []
    next_arrival = completed = current = 0
    segments: list[tuple[int, int, int]] = []
    completion = [0] * size

    def push_arrivals() -> None:
        nonlocal next_arrival, completed
        while next_arrival < size and release[arrivals[next_arrival]] <= current:
            job = arrivals[next_arrival]
            next_arrival += 1
            if remaining[job] == 0:
                completion[job] = release[job]
                completed += 1
            else:
                heapq.heappush(queue, (remaining[job], job))

    while completed < size:
        if not queue and next_arrival < size:
            current = max(current, release[arrivals[next_arrival]])
        push_arrivals()
        if not queue:
            continue
        _, job = heapq.heappop(queue)
        run = remaining[job]
        if next_arrival < size:
            run = min(run, release[arrivals[next_arrival]] - current)
        end = current + run
        if segments and segments[-1][0] == job and segments[-1][2] == current:
            segments[-1] = (job, segments[-1][1], end)
        else:
            segments.append((job, current, end))
        remaining[job] -= run
        current = end
        push_arrivals()
        if remaining[job] == 0:
            completion[job] = current
            completed += 1
        else:
            heapq.heappush(queue, (remaining[job], job))
    return max(completion, default=0), segments


def dispatch(processing: list[int], release: list[int], key) -> tuple[int, list[int]]:
    size = len(processing)
    done = [False] * size
    order: list[int] = []
    current = 0
    for _ in range(size):
        available = [job for job in range(size) if not done[job] and release[job] <= current]
        if not available:
            current = min(release[job] for job in range(size) if not done[job])
            available = [job for job in range(size) if not done[job] and release[job] <= current]
        chosen = min(available, key=lambda job: key(job, current))
        done[chosen] = True
        order.append(chosen)
        current += processing[chosen]
    return current, order


def atc(processing: list[int], release: list[int], due: list[int], weight: list[int]) -> tuple[int, list[int]]:
    size = len(processing)
    done = [False] * size
    order: list[int] = []
    current = 0
    for _ in range(size):
        available = [job for job in range(size) if not done[job] and release[job] <= current]
        if not available:
            current = min(release[job] for job in range(size) if not done[job])
            available = [job for job in range(size) if not done[job] and release[job] <= current]
        positive = [processing[job] for job in available if processing[job] > 0]
        mean = sum(positive) / len(positive) if positive else 0.0

        def rank(job: int) -> tuple[float, int]:
            if processing[job] == 0:
                return (-math.inf, job)
            slack = max(due[job] - current - processing[job], 0)
            penalty = slack / mean / 2 if mean else 0.0
            score = math.log(weight[job]) - math.log(processing[job]) - penalty
            return (-score, job)

        chosen = min(available, key=rank)
        done[chosen] = True
        order.append(chosen)
        current += processing[chosen]
    return current, order


def atcs(processing: list[int], release: list[int], due: list[int], weight: list[int], initial: list[int], setup: list[list[int]]) -> tuple[int, list[int]]:
    size = len(processing)
    done = [False] * size
    order: list[int] = []
    current = 0
    previous: int | None = None
    for _ in range(size):
        available = [job for job in range(size) if not done[job] and release[job] <= current]
        if not available:
            current = min(release[job] for job in range(size) if not done[job])
            available = [job for job in range(size) if not done[job] and release[job] <= current]
        positive_p = [processing[job] for job in available if processing[job] > 0]
        mean_p = sum(positive_p) / len(positive_p) if positive_p else 0.0
        setup_values = [initial[job] if previous is None else setup[previous][job] for job in available]
        positive_s = [value for value in setup_values if value > 0]
        mean_s = sum(positive_s) / len(positive_s) if positive_s else 0.0

        def rank(job: int) -> tuple[float, int, int]:
            setup_value = initial[job] if previous is None else setup[previous][job]
            if processing[job] == 0:
                return (-math.inf, setup_value, job)
            ready = current + setup_value
            slack = max(due[job] - ready - processing[job], 0)
            score = math.log(weight[job]) - math.log(processing[job])
            if mean_p:
                score -= slack / mean_p / 2
            if mean_s:
                score -= setup_value / mean_s
            return (-score, 0, job)

        chosen = min(available, key=rank)
        done[chosen] = True
        order.append(chosen)
        current += initial[chosen] if previous is None else setup[previous][chosen]
        current += processing[chosen]
        previous = chosen
    return current, order


def expected(case: Case) -> list[str]:
    processing, release, due, weight, initial, setup = case
    makespan, segments = srpt(processing, release)
    flat_segments = [value for segment in segments for value in segment]
    lines = [" ".join(map(str, [makespan, len(segments), *flat_segments]))]
    rules = [
        dispatch(processing, release, lambda job, _: (processing[job], job)),
        dispatch(processing, release, lambda job, now: (due[job] - now - processing[job], job)),
        dispatch(processing, release, lambda job, now: (0, 0.0, job) if processing[job] == 0 else (1, (due[job] - now) / processing[job], job)),
        dispatch(processing, release, lambda job, now: (0, 0.0, job) if processing[job] == 0 else (1, -(now - release[job]) / processing[job], job)),
        atc(processing, release, due, weight),
        atcs(processing, release, due, weight, initial, setup),
    ]
    lines.extend(" ".join(map(str, [finish, len(order), *order])) for finish, order in rules)
    return lines


def write_cases(path: Path, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for case in cases:
        processing, release, due, weight, initial, setup = case
        input_lines.append(str(len(processing)))
        for values in (processing, release, due, weight, initial):
            input_lines.append(" ".join(map(str, values)))
        input_lines.extend(" ".join(map(str, row)) for row in setup)
        output_lines.extend(expected(case))
    path.with_suffix(".in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    path.with_suffix(".out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    deterministic: list[Case] = [
        ([], [], [], [], [], []),
        ([0], [0], [0], [1], [0], [[0]]),
        ([8, 4, 2, 1], [0, 1, 2, 3], [20, 9, 7, 4], [1, 2, 3, 4], [1, 0, 2, 1], [[0, 2, 3, 1], [1, 0, 2, 4], [2, 1, 0, 3], [3, 2, 1, 0]]),
        ([0, 5, 0, 3], [0, 0, 2, 7], [0, 10, -2, 9], [5, 1, 7, 3], [2, 1, 0, 3], [[0, 3, 1, 2], [2, 0, 4, 1], [1, 2, 0, 3], [4, 1, 2, 0]]),
    ]
    write_cases(out_dir / "case_00", deterministic)
    rng = random.Random(2026071323)
    random_cases: list[Case] = []
    for _ in range(24):
        size = rng.randrange(1, 11)
        processing = [rng.randrange(0, 21) for _ in range(size)]
        release = [rng.randrange(0, 25) for _ in range(size)]
        due = [rng.randrange(-10, 80) for _ in range(size)]
        weight = [rng.randrange(1, 20) for _ in range(size)]
        initial = [rng.randrange(0, 8) for _ in range(size)]
        setup = [[rng.randrange(0, 8) for _ in range(size)] for _ in range(size)]
        for job in range(size):
            setup[job][job] = 0
        random_cases.append((processing, release, due, weight, initial, setup))
    write_cases(out_dir / "case_01", random_cases)


if __name__ == "__main__":
    main()

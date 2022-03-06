from typing import List, Dict
from dataclasses import dataclass
from datetime import timedelta


@dataclass
class Data:
    durations: List[timedelta]
    repetitions: List[int]
    results: List[bool]


def parse_data(filename: str) -> Data:
    # Vytvorime pole 3 poli
    data = [[] for _ in range(3)]

    with open(filename) as f:
        lines = f.readlines()

        # Smazany jmena sloupcu
        del lines[0]

        for line in lines:
            # Ze radku je smazan \n charakter (strip), a pote je rozdelen pomoci ','
            values = line.strip().split(",")
            # Hodnoty prevedene ze stringu na integer
            values = [int(val) for val in values]

            # trvani
            delta = timedelta(hours=values[1], minutes=values[2], seconds=values[3])
            data[0].append(delta)

            # opakovani
            data[1].append(values[4])

            # vysledek
            data[2].append(bool(values[5]))

    # Pomoci * "rozbalime" vsechny polozky pole do konstruktoru objektu Data
    return Data(*data)


def get_longest_streak(results: List[bool]) -> int:
    # Vytvorime si kopii vysledku a pridame False na konec
    # kdyby bylo posledni pomodoro True, tak se streak nezapocita, protoze se zapocitava jen kdyz narazi na False
    _results = results + [False]

    # nejvyssi streak
    highest = 0
    # streak ktery ted pocitame
    current = 0
    for res in _results:
        # Pokud je vysledek pravdivy, zvetsime streak
        if res:
            current += 1
        # Pokud narazime na False, ukoncime streak
        else:
            # Pokud je streak vetsi nez nas nejvyssi, zmenime ho
            if current > highest:
                highest = current

            # Vynulujeme streak
            current = 0

    return highest


def analyze_data(data: Data) -> Dict[str, int]:
    # Slovnik pro ukladani vysledku
    stats = {}
    stats["Average duration"] = sum([delta.total_seconds() / 60 for delta in data.durations]) / len(data.durations)
    stats["Average repetitions "] = sum(data.repetitions) / len(data.repetitions)
    stats["Longest success streak"] = get_longest_streak(data.results)

    # Vyfiltrujeme ty vysledky ktere jsou pozitivni, vydelime poctem vsech a vynasobime 100
    stats["Success rate"] = (len([res for res in data.results if res]) / len(data.results)) * 100
    stats["Longest duration"] = max([delta.total_seconds() / 60 for delta in data.durations])
    stats["Most repetitions"] = max(data.repetitions)

    return stats


def write_to_file(filename: str, stats: Dict[str, int]):
    with open(filename, "w", encoding="utf8") as f:
        for name, value in stats.items():
            f.write(f"{name}: {value}\n")


INPUT = "stats.csv"
OUTPUT = "out.txt"

data = parse_data(INPUT)
stats = analyze_data(data)
write_to_file(OUTPUT, stats)

from random import randint


def generate_data(filename: str):
    with open(filename, "w", encoding="utf8") as f:
        f.write("Pomodoro,Hours,Minutes,Seconds,Repats,Success\n")
        for i in range(randint(10, 1000)):
            hours = randint(0, 3)
            minutes = randint(0, 59)
            seconds = randint(0, 59)
            repetitions = randint(0, 15)
            res = randint(0, 1)
            f.write(f"{i},{hours},{minutes},{seconds},{repetitions},{res}\n")


if __name__ == "__main__":
    GENERATED_FILE = "stats.csv"
    generate_data(GENERATED_FILE)
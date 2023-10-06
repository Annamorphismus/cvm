import matplotlib.pyplot as plt
import argparse
import os



# Liest Daten von einer Datei des "naiven" Algorithmus.
def read_naive_data_from_file(file_path):
    parsed_data = {}

    with open(file_path, "r") as f:
        lines = f.readlines()

        for line in lines:
            # Überspringt leere Zeilen.
            if not line.strip():
                continue
            parts = line.split()
            x, epsilon, delta = map(int, parts[0].split("/"))
            runtime = float(parts[1]) / 1000000

            # Berücksichtigt nur Datenpunkte mit spezifischen Werten für epsilon und delta.
            if epsilon != 5:
                continue

            if delta != 100:
                continue

            # Speichert die Daten.
            key = (epsilon / 10, delta * 0.0001)

            if key not in parsed_data:
                parsed_data[key] = []
            parsed_data[key].append((x, runtime))

    return parsed_data


# Liest Daten von einer Datei des Knuth Algorithmus.
def read_knuth_data_from_file(file_path):
    parsed_data = {}
    with open(file_path, "r") as f:
        lines = f.readlines()

        for line in lines:
            # Überspringt leere Zeilen.
            if not line.strip():
                continue
            parts = line.split()
            x, s = map(int, parts[0].split("/"))
            runtime = float(parts[1]) / 1000000

            # Speichert die Daten.
            key = s
            if key not in parsed_data:
                parsed_data[key] = []
            parsed_data[key].append((x, runtime))

    return parsed_data


# Liest alle Daten für eine gegebene Bitgröße.
def read_all_data(bits):
    all_files = [f for f in os.listdir(".")]
    knuth = [
        x
        for x in all_files
        if "knuth" in x and str(bits) in x and not x.endswith("pdf")
    ]
    naive = [
        x
        for x in all_files
        if "naive" in x and str(bits) in x and not x.endswith("pdf")
    ]

    data = {}
    for f in knuth:
        d = read_knuth_data_from_file(f)

        # Vereint die Daten.
        data = d | data

    for f in naive:
        d = read_naive_data_from_file(f)
        print(d)

        # Vereint die Daten.
        data = d | data

    return data


# Zeichnet alle Datenpunkte.
def plot_all(data):
    # Plot
    plt.figure(figsize=(14, 8))
    for l, values in data.items():
        x_vals, y_vals = zip(*sorted(values, key=lambda x: x[0]))
        thick = False

        try:
            label = f"ε={l[0]}, δ={l[1]}"
            thick = True
        except:
            label = f"|B|={l}"

        if thick:
            plt.plot(x_vals, y_vals, label=label, linewidth=5)
        else:
            plt.plot(x_vals, y_vals, label=label)

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Value of x")
    plt.ylabel("Runtime (ms)")
    plt.title("Knuth vs. Naive")
    plt.legend(loc="upper left", bbox_to_anchor=(1, 1))
    plt.grid(True, which="both", ls="--", linewidth=0.5)
    plt.tight_layout()

    plt.show()


# Hauptfunktionsaufruf, wenn das Skript direkt ausgeführt wird.
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot function from data file.")
    parser.add_argument("bits", help="Path to the data file.")

    args = parser.parse_args()

    data = read_all_data(args.bits)
    plot_all(data)

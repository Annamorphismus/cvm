import matplotlib.pyplot as plt
import argparse


# Diese Funktion liest die Daten des Knuth-Algorithmus
def read_knuth_data_from_file(file_path):
    # Öffnet die Datei zum Lesen.
    with open(file_path, "r") as f:
        lines = f.readlines()

    parsed_data = {}
    for line in lines:
        # Überspringt leere Zeilen.
        if not line.strip():
            continue
        parts = line.split()
        x, s = map(int, parts[0].split('/'))
        runtime = float(parts[1]) / 1000000 # Konvertiert die Laufzeit in Sekunden.

        # Speichert die Daten im parsed_data-Dictionary.
        key = s
        if key not in parsed_data:
            parsed_data[key] = []
        parsed_data[key].append((x, runtime))

    return parsed_data


# Diese Funktion plottet die Daten.
def plot_naive_only(data):

    # Erstellt eine neue Figur mit einer Größe von 14x8 Zoll.
    plt.figure(figsize=(14, 8))

    # Geht durch jedes Element in den Daten und zeichnet es.
    for s, values in data.items():
        x_vals, y_vals = zip(*sorted(values, key=lambda x: x[0]))
        plt.plot(x_vals, y_vals, label=f"|B|={s}")


    # Setzt die x- und y-Achse auf eine logarithmische Skala.
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Value of x") # Beschriftung der x-Achse.
    plt.ylabel("Runtime (ms)") # Beschriftung der y-Achse.
    plt.title("Runtime vs. x for different buffersizes") # Titel des Plots.
    plt.legend(loc='upper left', bbox_to_anchor=(1, 1)) # Legende-Einstellungen.
    plt.grid(True, which="both", ls="--", linewidth=0.5) # Rastereinstellungen.
    plt.tight_layout() # Anpassen des Layouts, damit alles sichtbar ist.

    # Zeigt den Plot an.
    plt.show()


# Hauptfunktionsaufruf, wenn das Skript direkt ausgeführt wird.
if __name__ == "__main__":

    # ArgumentParser erlaubt es, Befehlszeilenargumente zu parsen.
    parser = argparse.ArgumentParser(description="Plot function from data file.")
    # Argument für den Dateipfad.
    parser.add_argument("file_path", help="Path to the data file.")

    # Parsen der Argumente.
    args = parser.parse_args()

    # Liest Daten aus der Datei.
    parsed_data = read_knuth_data_from_file(args.file_path)

    # Zeichnet die Daten.
    plot_naive_only(parsed_data)

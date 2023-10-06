import matplotlib.pyplot as plt
import argparse


# Diese Funktion liest die Daten des Naiven Algorithmus aus einer gegebenen Datei.
def read_naive_data_from_file(file_path):
    # Öffnet die Datei zum Lesen.
    with open(file_path, "r") as f:
        lines = f.readlines()

    parsed_data = {}
    for line in lines:
        # Überspringt leere Zeilen.
        if not line.strip():
            continue
        # Teilt die Zeile in ihre Teile auf.
        parts = line.split()

        # Extrahiert x, epsilon und delta Werte aus dem ersten Teil der Zeile.
        x, epsilon, delta = map(int, parts[0].split('/'))

        # Konvertiert die Laufzeit in Sekunden.
        runtime = float(parts[1]) / 1000000

        # Erzeugt einen Schlüssel für das parsed_data Wörterbuch aus epsilon und delta.
        key = (epsilon / 10 , delta * 0.0001)
        if key not in parsed_data:
            parsed_data[key] = []
        parsed_data[key].append((x, runtime))

    return parsed_data


# Diese Funktion plottet die "naive"-Daten.
def plot_naive_only(data):

    # Erstellt eine neue Figur mit einer Größe von 14x8 Zoll.
    plt.figure(figsize=(14, 8))

    # Geht durch jedes Datenpaar und zeichnet es auf den Plot.
    for (epsilon, delta), values in data.items():
        x_vals, y_vals = zip(*sorted(values, key=lambda x: x[0]))
        plt.plot(x_vals, y_vals, label=f"ε={epsilon}, δ={delta}")

    # Setzt die x- und y-Achse auf eine logarithmische Skala.
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Value of x") # Beschriftung der x-Achse
    plt.ylabel("Runtime (ms)") # Beschriftung der y-Achse.
    plt.title("Runtime vs. x for different ε-δ combinations") # Titel des Plots.
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
    parsed_data = read_naive_data_from_file(args.file_path)

    # Zeichnet die Daten.
    plot_naive_only(parsed_data)

import matplotlib.pyplot as plt
import argparse

def read_knuth_data_from_file(file_path):
    with open(file_path, "r") as f:
        lines = f.readlines()

    parsed_data = {}
    for line in lines:
        if not line.strip():
            continue
        parts = line.split()
        x, s = map(int, parts[0].split('/'))
        runtime = float(parts[1]) / 1000000

        # Store data
        key = s
        if key not in parsed_data:
            parsed_data[key] = []
        parsed_data[key].append((x, runtime))

    return parsed_data

def plot_naive_only(data):

    # Plot
    plt.figure(figsize=(14, 8))
    for s, values in data.items():
        x_vals, y_vals = zip(*sorted(values, key=lambda x: x[0]))
        plt.plot(x_vals, y_vals, label=f"|B|={s}")

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Value of x")
    plt.ylabel("Runtime (ms)")
    plt.title("Runtime vs. x for different buffersizes")
    plt.legend(loc='upper left', bbox_to_anchor=(1, 1))
    plt.grid(True, which="both", ls="--", linewidth=0.5)
    plt.tight_layout()

    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot function from data file.")
    parser.add_argument("file_path", help="Path to the data file.")

    args = parser.parse_args()
    parsed_data = read_knuth_data_from_file(args.file_path)

    plot_naive_only(parsed_data)

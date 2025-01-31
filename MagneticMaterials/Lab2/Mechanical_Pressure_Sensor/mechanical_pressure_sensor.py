import matplotlib.pyplot as plot
import csv

csv_files = ["5g.csv", "20g.csv", "100g.csv", "200g.csv"]
if __name__ == "__main__":
    for csv_file in csv_files:
        seconds = []
        voltage = []
        with open(csv_file, "r") as file:
            csvreader = csv.reader(file)
            next(csvreader)
            for row in csvreader:
                seconds.append(float(row[0]))
                voltage.append(float(row[1]))
            plot.plot(seconds, voltage, color="black")
            plot.xlabel("Time(s)")
            plot.ylabel("Voltage(V)")
            plot.title(csv_file.strip(".csv"))
            plot.show()


amplitude = [0.0078, 0.0187, 0.0275, 0.0298]
weight = ['5', '20', '100', '200']
plot.plot(weight, amplitude)
plot.xlabel("Mechanical Stress(g)")
plot.ylabel("Amplitude (V)")
plot.show()
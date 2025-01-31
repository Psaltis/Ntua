import matplotlib.pyplot as pyplot

if __name__ == "__main__":
    magnetic_core_position = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
    vpp1khz = [1.22, 1.18, 1.1, 1.01, 0.928, 0.872, 0.784, 0.696, 0.576, 0.448, 0.32]
    vpp5khz = [3.14, 2.96, 2.74, 2.56, 2.38, 2.18, 1.92, 1.68, 1.34, 1.08, 0.928]

    fig, ax= pyplot.subplots()
    ax.plot(magnetic_core_position, vpp1khz, "ob", label = "Vpp@1kHz")
    ax.plot(magnetic_core_position, vpp5khz, "ob",color = 'g', label = "Vpp@5kHz")
    ax.legend()
    ax.set(xlabel = 'Displacement(mm)', ylabel = 'Voltage(V)', title = 'Peak to peak voltages')
    pyplot.show()  

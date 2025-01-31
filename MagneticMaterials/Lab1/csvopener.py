import matplotlib.pyplot as plot 
import csv
import scipy.integrate as integrate

if __name__ == "__main__":
# this is for the first magnetic core material
# 100 Hz
    seconds = []
    input_current = []
    output_voltage = []
    with open('1_100Hz.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
# plot Vout(t) Iin(t)       
        plot.plot(seconds, output_voltage, color = 'g', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'black', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@100Hz")
        plot.legend()
        plot.show()
# print integral on terminal
        integralbitch = integrate.trapz(output_voltage, seconds)
        print(integralbitch)
        integral = integrate.cumtrapz(output_voltage, seconds, initial=0)
        plot.plot(seconds, integral)
        plot.show()
# plot Hysterisis Loop
        plot.plot(input_current, integral, ".b")
        plot.xlabel("Iin")
        plot.title( "Hysterisis Loop@100Hz")
        plot.show()
# 1kHz
    seconds = []
    input_current = []
    output_voltage = []
    with open('1_1kHz.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
# plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@1kHz")
        plot.legend()
        plot.show()
# print integral on terminal
        integralbitch = integrate.trapz(output_voltage, seconds)
        print(integralbitch)
        integral = integrate.cumtrapz(output_voltage, seconds, initial=0)
        plot.plot(seconds, integral, ".b")
        plot.show()
# plot Hysterisis Loop
        plot.plot(input_current, integral, ".b")
        plot.xlabel("Iin")
        plot.title( "Hysterisis Loop@1kHz")
        plot.show()
#2_2V
    seconds = []
    input_current = []
    output_voltage = []
    with open('2_2V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
# plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@2V")
        plot.legend()
        plot.show()
 # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color = 'b', label = "Vout(In)")
        plot.xlabel("Iin")
        plot.title( "Vout@2V")
        plot.legend()
        plot.show()

#2_4V
    seconds = []
    input_current = []
    output_voltage = []
    with open('2_4V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@4V")
        plot.legend()
        plot.show()
  # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color = 'b', label = "Vout(In)")
        plot.xlabel("Iin")
        plot.title( "Vout@4V")
        plot.legend()
        plot.show()
#2_6V
    seconds = []
    input_current = []
    output_voltage = []
    with open('2_6V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@6V")
        plot.legend()
        plot.show()
 # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color = 'b', label = "Vout(In)")
        plot.xlabel("Iin")
        plot.title( "Vout@6V")
        plot.legend()
        plot.show()

#2_8V
    seconds = []
    input_current = []
    output_voltage = []
    with open('2_8V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)") 
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@8V")
        plot.legend()
        plot.show()
        # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color='b', label="Vout(In)")
        plot.xlabel("Iin")
        plot.title("Vout@8V")
        plot.legend()
        plot.show()

#2_10V
    seconds = []
    input_current = []
    output_voltage = []
    with open('2_10V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)") 
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@10V")
        plot.legend()
        plot.show()
        # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color='b', label="Vout(In)")
        plot.xlabel("Iin")
        plot.title("Vout@10V")
        plot.legend()
        plot.show()



#this is for the second magnetic core material
    #100Hz
    seconds = []
    input_current = []
    output_voltage = []
    with open('3_100Hz.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'orange', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@100Hz")
        plot.legend()
        plot.show()
# print integral on terminal
        integralbitch = integrate.trapz(output_voltage, seconds)
        print(integralbitch)
        integral = integrate.cumtrapz(output_voltage, seconds, initial=0)
        plot.plot(seconds, integral, ".b")
        plot.show()
# plot Hysterisis Loop
        plot.plot(input_current, integral, ".b")
        plot.xlabel("Iin")
        plot.title( "Hysterisis Loop@100Hz")
        plot.show()

    #1kHz
    seconds = []
    input_current = []
    output_voltage = []
    with open('3_1kHz.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
# plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@1kHz")
        plot.legend()
        plot.show()
# print integral on terminal
        integralbitch = integrate.trapz(output_voltage, seconds)
        print(integralbitch)
        integral = integrate.cumtrapz(output_voltage, seconds, initial=0)
        plot.plot(seconds, integral, ".b")
        plot.show()
# plot Hysterisis Loop
        plot.plot(input_current, integral, ".b")
        plot.xlabel("Iin")
        plot.title( "Hysterisis Loop@1kHz")
        plot.show()

#4_2V
    seconds = []
    input_current = []
    output_voltage = []
    with open('4_2V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
# plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)") 
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@2V")
        plot.legend()
        plot.show()
 # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color = 'b', label = "Vout(In)")
        plot.xlabel("Iin")
        plot.title( "Vout@2V")
        plot.legend()
        plot.show()

#4_4V
    seconds = []
    input_current = []
    output_voltage = []
    with open('4_4V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)") 
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@4V")
        plot.legend()
        plot.show()
        # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color='b', label="Vout(In)")
        plot.xlabel("Iin")
        plot.title("Vout@4V")
        plot.legend()
        plot.show()

#4_6V
    seconds = []
    input_current = []
    output_voltage = []
    with open('4_6V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t)
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)") 
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@6V")
        plot.legend()
        plot.show()
        # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color='b', label="Vout(In)")
        plot.xlabel("Iin")
        plot.title("Vout@6V")
        plot.legend()
        plot.show()

#4_8V
    seconds = []
    input_current = []
    output_voltage = []
    with open('4_8V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)") 
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@8V")
        plot.legend()
        plot.show()
        # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color='b', label="Vout(In)")
        plot.xlabel("Iin")
        plot.title("Vout@8V")
        plot.legend()
        plot.show()

#4_10V
    seconds = []
    input_current = []
    output_voltage = []
    with open('4_10V.csv', 'r') as file:
        csvreader= csv.reader(file)
        next(csvreader)
        for row in csvreader:
            seconds.append(float(row[0]))
            input_current.append(float(row[1])/1000)
            output_voltage.append(float(row[2]))
 # plot Vout(t) Iin(t) 
        plot.plot(seconds, output_voltage, color = 'b', label = "Vout(t)")
        plot.plot(seconds, input_current, color = 'r', label = "Iin(t)")
        plot.xlabel("Time(s)")
        plot.title( "In(t) and Vout(t)@10V")
        plot.legend()
        plot.show()
 # plot Vout(t) Iin(t)
        plot.plot(input_current, output_voltage, color = 'b', label = "Vout(In)")
        plot.xlabel("Iin")
        plot.title( "Vout@10V")
        plot.legend()
        plot.show()


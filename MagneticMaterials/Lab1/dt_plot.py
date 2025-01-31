import matplotlib.pyplot as pyplot 

vdc_offset = [-3, -2, -1, 0 , 1, 2, 3]

dt_1khz = [-0.4, -0.3, -0.1, 0, 0.14, 0.3, 0.42]

dt_5khz = [-0.06, -0.055, -0.025, 0, 0.03, 0.06, 0.06]

pyplot.plot(vdc_offset, dt_1khz, "ob", label = '@1kHz')
pyplot.plot(vdc_offset, dt_5khz, "ob", color = 'red', label = '@5kHz')
pyplot.xlabel('DC Offset')
pyplot.ylabel ('Δt') 
pyplot.title('Δt(Vdc)')
pyplot.legend()
pyplot.show()
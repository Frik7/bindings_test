# This is the python module binded to the equivalent c++ executreable.
import MonteCarlo as mc

# The below function is analougus to MCOptionPricer in MCOptionPricer.cpp
x = mc.OptionPricer(1, 100, 95, 0.2, 0.04, 3000000)
print(f'The call price is {round(x[0], 2)}, and the put price is {round(x[1], 2)}')

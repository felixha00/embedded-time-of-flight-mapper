import serial
from decimal import Decimal
import math

#Modify the following line with your own serial port details
#   Currently set COM3 as serial port at 115.2kbps 8N1
#   Refer to PySerial API for other options.  One option to consider is
#   the "timeout" - allowing the program to proceed if after a defined
#   timeout period.  The default = 0, which means wait forever.

s = serial.Serial("COM5", 115200)

Angles = []
Distances = []
Displacement = []
angle = ""
distance = ""
st = ""
x = 0;
increment = 250


print("Opening: " + s.name)

s.write(b'1')           #This program will send a '1' or 0x31 

while (True):
    input = s.read().decode('UTF-8') # decodes each byte 

    if input == '!': # checks if its at the end of the angles transmission
        Angles.append(int(st)/(256/45));
        print ("Angle: "+st)
        Displacement.append(x)
        if st == '2048': # if at the end of the 
            x = x+increment
        print ("Displacement: "+str(x))
        st = ""
        
           
    elif input == '?': # checks if its at the end of the distances transmission
        Distances.append(int(st));
        print ("Distance: "+st)
        st = ""
    elif input == '&': # receives this character only if button is pressed and transmission stops.
        break
    else:        
        st += input # appends the character to the end of the string if no end char is found
            
print("Closing: " + s.name)
s.close();

# FILE WRITING
f = open("tof_data.txt", "w")
for i in range(len(Distances)):
    f.write(
        # x y z\n
        str(Displacement[i])+" "+ 
        str(Distances[i]*math.sin(float(math.radians(Angles[i]))))+ " "+
        str(Distances[i]*math.cos(float(math.radians(Angles[i]))))+ "\n"  
    
    )
f.close()


import math

numsides = 18
print('vertex')
for i in range(numsides):
    scaledcurr = i / numsides
    scalednext = (i + 1) / numsides
    x1 = 1 * math.cos(2 * math.pi * scaledcurr)
    x2 = 1 * math.cos(2 * math.pi * scalednext)
    y1 = 1 * math.sin(2 * math.pi * scaledcurr)
    y2 = 1 * math.sin(2 * math.pi * scalednext)
    print('{' + f'{x1:1.3f}f, {y1:1.3f}f, 0.000f' + '},')
    print('{' + f'{x2:1.3f}f, {y2:1.3f}f, 0.000f' + '},')
    print('{0.000f, 0.000f, 0.000f},')
    

print('\nfragment')
for i in range(numsides):
    print('{1.000f, 0.000f, 0.000f},')
    print('{1.000f, 0.000f, 0.000f},')
    print('{1.000f, 0.000f, 0.000f},')


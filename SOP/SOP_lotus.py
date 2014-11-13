import math

# This code is called when instances of this SOP cook.
node = hou.pwd()
geo = node.geometry()

def bottom(num, vOffset, nOffset):
    z = 0.001
    # intersection point between ll_se and ll_ne
    a = 1
    b = -2
    c = -0.754999
    discriminant = b * b - 4.0 * a * c
    x1 = (2 + math.sqrt(discriminant)) / 2.0
    y1 = 0.0
    p1 = [x1, y1, z] # we need only one of them
    # intersection point between clip_base_m and ll_ne
    m = (z * z - 5 * z + 8.25) / 2.0
    a = 2
    b = -2 * m
    c = m * m - 9
    discriminant = b * b - 4.0 * a * c
    x2 = (2 * m + math.sqrt(discriminant)) / 4.0
    x3 = (2 * m - math.sqrt(discriminant)) / 4.0
    y2 = m - x2
    y3 = m - x3
    p2 = [x2, y2, z]
    p3 = [x3, y3, z]
    # cylinder top at bottom of the lotus
    vOffset, nOffset, part1 = cylinderTop(vOffset, nOffset, num, p1, p2)
    # cylinder sides at bottom of the lotus
    vOffset, nOffset, part2 = cylinderSides(vOffset, nOffset, num, p2, p3)
    # assemble parts for lower rim
    points = []
    for point in part1:
        # simply copy
        points.append(point)
    for point in part2:
        # simply copy
        points.append(point)
    for index in xrange(len(part1)):
        rIndex = len(part1) - 1 - index
        point = part1[rIndex] # reverse order
        # swap x and y
        points.append([point[1], point[0], point[2]])
    return vOffset, nOffset, points

def cylinderSides(vOffset, nOffset, num, p1, p2):
    rim = [] # to return the rim
    nVertices = 0
    nNormals = 0
    # extract coords
    x1 = p1[0]
    y1 = p1[1]
    z1 = p1[2]
    x2 = p2[0]
    y2 = p2[1]
    z2 = p2[2]
    z = z2
    # angle between x-axis and p1
    rad = math.atan(y1 / x1)
    # how many points to intersection?
    num1 = int(num * (4.0 * rad) / math.pi)
    # how many points from intersection to second intersection?
    num2 = 2 * (num - num1) + 1
    # points on cylinder
    rad1 = math.atan(y1 / x1)
    rad2 = math.atan(y2 / x2)
    points = []
    for i in xrange(num2):
        rad = rad1 + i * (rad2 - rad1) / float(num2 - 1)
        cos = math.cos(rad)
        sin = math.sin(rad)
        x = 3.0 * cos
        y = 3.0 * sin
        nVertices = nVertices + 1
        points.append([x, y, z])
        # calculate points on cylinder (with z != 0.001)
        a = 1
        b = -5.0
        c = 8.25 - 2 * (x + y)
        discriminant = b * b - 4.0 * a * c
        z1 = (5.0 + math.sqrt(discriminant)) / 2.0
        z2 = (5.0 - math.sqrt(discriminant)) / 2.0
        if i != 0 and i != num2 - 1:
            nVertices = nVertices + 1
            points.append([x, y, z2])
            rim.append([x, y, z2])
    return vOffset + nVertices, nOffset + nNormals, rim

def cylinderTop(vOffset, nOffset, num, p1, p2):
    rim = [] # to return the rim
    print "# cylinder top at bottom of the lotus"
    nVertices = 0
    nNormals = 0
    # extract coords
    x1 = p1[0]
    y1 = p1[1]
    z1 = p1[2]
    x2 = p2[0]
    y2 = p2[1]
    z2 = p2[2]
    if z1 == z2:
        z = z1
    # angle between x-axis and p1
    rad = math.atan(y2 / x2)
    # how many points to intersection?
    num1 = int(num * (4.0 * rad) / math.pi)
    # how many points from intersection to 45 degrees?
    num2 = num - num1 + 1
    # points on ll_ne with z = 0.001
    k2 = 9.0 - 6.245001
    k = math.sqrt(k2)
    rad1 = math.atan((y1 - 1.0) / (x1 - 1.0))
    rad2 = math.atan((y2 - 1.0) / (x2 - 1.0))
    nVertices = nVertices + 1
    for i in xrange(num1):
        rad = rad1 + i * (rad2 - rad1) / float(num1 - 1)
        cos = math.cos(rad)
        sin = math.sin(rad)
        x = 1.0 + k * cos
        y = 1.0 + k * sin
        nVertices = nVertices + 1
        rim.append([x, y, z])
    rad1 = math.atan(y2 / x2)
    rad2 = math.pi / 4.0
    for i in xrange(1, num2): # omit first vertex (was written already above)
        rad = rad1 + i * (rad2 - rad1) / float(num2 - 1)
        cos = math.cos(rad)
        sin = math.sin(rad)
        x = 3.0 * cos
        y = 3.0 * sin
        nVertices = nVertices + 1
    return vOffset + nVertices, nOffset + nNormals, rim

# Add code to modify the contents of geo.
vOffset = 0
nOffset = 0
vOffset, nOffset, lowerRim = bottom(16, vOffset, nOffset)
bottom = geo.createPolygon()
for position in lowerRim:
    point = geo.createPoint()
    point.setPosition((position[0], position[1], position[2]))
    bottom.addVertex(point)

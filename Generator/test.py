import math

myfile = open("parking.inp", "r")

least_parking, parking_count = list(map(int, myfile.readline().split()))

current_parking = least_parking

using_parking = 0

parking = []

for x in range(least_parking):
    parking.append(0)

for x in range(parking_count):
    car_inout = int(myfile.readline())
    if (using_parking == current_parking and car_inout > 0):
        for y in range(current_parking):
            parking.append(0)

        current_parking = current_parking * 2

    if (car_inout > 0):

        for y in range(current_parking):

            if (parking[y] == 0):
                parking[y] = car_inout

                using_parking = using_parking + 1

                break

    else:

        for y in range(current_parking):

            if (parking[y] == abs(car_inout)):
                parking[y] = 0

                using_parking = using_parking - 1

                break

    if (using_parking <= math.floor(current_parking / 3) and current_parking > least_parking):

        parking_change_count = 0

        for y in range(current_parking):

            if (parking[y] != 0 and parking_change_count == y):

                parking_change_count = parking_change_count + 1

            elif (parking[y] != 0):

                parking[parking_change_count] = parking[y]

                parking[y] = 0

                parking_change_count = parking_change_count + 1

            if (parking_change_count >= using_parking):
                break

        current_parking = math.floor(current_parking / 2)

        for y in range(current_parking):
            del parking[-1]

    elif (using_parking <= math.floor(current_parking / 3)):

        parking_change_count = 0

        for y in range(current_parking):

            if (parking[y] != 0 and parking_change_count == y):

                parking_change_count = parking_change_count + 1

            elif (parking[y] != 0):

                parking[parking_change_count] = parking[y]

                parking[y] = 0

                parking_change_count = parking_change_count + 1

            if (parking_change_count >= using_parking):
                break

myfile.close()

myfile = open("parking.out", "w")

for x in range(current_parking):
    if (parking[x] != 0):
        myfile.write(str(x + 1))
        myfile.write(" ")
        myfile.write(str(parking[x]))
        myfile.write("\n")

myfile.close()
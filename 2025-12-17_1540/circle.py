#!/usr/bin/env python3
import sys
import math

def main():
    # Syntax: circle <x> <y> <z> <radius> <num_points> <axis>
    # Beispiel: circle 1 2 5 100 256 y

    if len(sys.argv) != 7:
        print("Usage: python circle.py <x> <y> <z> <radius> <num_points> <axis>")
        print("axis: x, y oder z")
        print("Beispiel: python circle.py 1 2 5 100 256 y")
        sys.exit(1)

    try:
        center_x = float(sys.argv[1])
        center_y = float(sys.argv[2])
        center_z = float(sys.argv[3])
        radius = float(sys.argv[4])
        num_points = int(sys.argv[5])
        axis = sys.argv[6].lower()
    except ValueError:
        print("Fehler: Alle Werte müssen Zahlen sein (außer Achse).")
        sys.exit(1)

    if axis not in ['x', 'y', 'z']:
        print("Achse muss 'x', 'y' oder 'z' sein.")
        sys.exit(1)

    if num_points < 3:
        print("Mindestens 3 Punkte erforderlich.")
        sys.exit(1)

    # Winkel-Schritt in Radiant
    angle_step = 2 * math.pi / num_points

    for i in range(num_points):
        angle = i * angle_step

        # Berechne die Koordinaten je nach Achse
        if axis == 'x':
            # Kreis in YZ-Ebene (X konstant)
            y = center_y + radius * math.cos(angle)
            z = center_z + radius * math.sin(angle)
            x = center_x
        elif axis == 'y':
            # Kreis in XZ-Ebene (Y konstant)
            x = center_x - radius * math.cos(angle)
            z = center_z + radius * math.sin(angle)
            y = center_y
        else:  # axis == 'z'
            # Kreis in XY-Ebene (Z konstant)
            x = center_x - radius * math.cos(angle)
            y = center_y + radius * math.sin(angle)
            z = center_z

        # Ausgabe: x,y,z (mit 6 Nachkommastellen, falls nötig)
        print(f"{x:.6f},{y:.6f},{z:.6f}")

if __name__ == "__main__":
    main()

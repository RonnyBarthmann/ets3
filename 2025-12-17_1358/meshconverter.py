#!/usr/bin/env python3
# mesh_to_triangles_with_gray.py
# Usage: python mesh_to_triangles_with_gray.py input.obj output.txt
#        python mesh_to_triangles_with_gray.py input.stl output.txt

import sys
import numpy as np
import random
import struct
import re

def read_obj(filename):
    vertices = []
    faces = []
    
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split()
            if parts[0] == 'v' and len(parts) >= 4:
                try:
                    x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
                    vertices.append((x, y, z))
                except:
                    pass
            elif parts[0] == 'f':
                # Unterstützt f 1 2 3, f 1//1 2//2 3//3, f 1/1/1 2/2/2 usw.
                idx = []
                for p in parts[1:]:
                    vertex_idx = int(p.split('/')[0]) - 1  # OBJ ist 1-basiert
                    if vertex_idx < 0:
                        vertex_idx += len(vertices)  # negative Indizes erlaubt
                    idx.append(vertex_idx)
                # Triangulieren von Quads/N-Gons (einfachster Fan-Ansatz)
                for i in range(1, len(idx)-1):
                    faces.append((idx[0], idx[i], idx[i+1]))
    
    return np.array(vertices, dtype=np.float32), faces

def read_stl_ascii(filename):
    vertices = []
    faces = []
    
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.read().lower()
    
    # Einfache Regex-basierte Extraktion der vertex-Zeilen
    vertex_pattern = re.compile(r'vertex\s+([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)')
    matches = vertex_pattern.findall(content)
    
    for i in range(0, len(matches), 3):
        v1 = (float(matches[i][0]), float(matches[i][1]), float(matches[i][2]))
        v2 = (float(matches[i+1][0]), float(matches[i+1][1]), float(matches[i+1][2]))
        v3 = (float(matches[i+2][0]), float(matches[i+2][1]), float(matches[i+2][2]))
        # STL definiert Dreiecke direkt → keine shared vertices
        idx1 = len(vertices)
        idx2 = idx1 + 1
        idx3 = idx1 + 2
        vertices.extend([v1, v2, v3])
        faces.append((idx1, idx2, idx3))
    
    return np.array(vertices, dtype=np.float32), faces

def read_stl_binary(filename):
    vertices = []
    faces = []
    
    with open(filename, 'rb') as f:
        header = f.read(80)
        num_triangles = struct.unpack('<I', f.read(4))[0]
        
        for _ in range(num_triangles):
            # Normalvektor (ignorieren)
            f.read(12)
            # Drei Vertices
            v1 = struct.unpack('<fff', f.read(12))
            v2 = struct.unpack('<fff', f.read(12))
            v3 = struct.unpack('<fff', f.read(12))
            # Attribute byte count (meist 0)
            f.read(2)
            
            idx1 = len(vertices)
            idx2 = idx1 + 1
            idx3 = idx1 + 2
            vertices.extend([v1, v2, v3])
            faces.append((idx1, idx2, idx3))
    
    return np.array(vertices, dtype=np.float32), faces

def read_stl(filename):
    # Prüfen, ob ASCII oder Binär
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            first_line = f.readline().lower()
            if 'solid' in first_line:
                # Könnte ASCII sein – versuchen zu parsen
                f.seek(0)
                test_content = f.read(1024).lower()
                if 'facet' in test_content and 'vertex' in test_content:
                    return read_stl_ascii(filename)
    except:
        pass
    
    # Wenn ASCII fehlschlägt oder nicht erkannt → Binär versuchen
    return read_stl_binary(filename)

def generate_random_gray_colors(num_vertices):
    """Erzeugt pro Vertex einen zufälligen Grauwert (R=G=B), als uint32 0xRRGGBB"""
    colors = []
    for _ in range(num_vertices):
        gray = random.randint(0, 255)
        color_uint32 = (gray << 16) | (gray << 8) | gray
        colors.append(color_uint32)
    return colors

def main():
    if len(sys.argv) != 3:
        print("Usage: python mesh_to_triangles_with_gray.py <input_mesh> <output.txt>")
        print("Supported: .obj, .stl (ASCII und binary)")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    ext = input_file.lower().split('.')[-1]
    
    if ext == 'obj':
        verts, faces = read_obj(input_file)
    elif ext == 'stl':
        verts, faces = read_stl(input_file)
    else:
        print(f"Unbekanntes Format: {ext}")
        sys.exit(1)
    
    print(f"Geladen: {len(verts)} Vertices, {len(faces)} Dreiecke")
    
    # Zufällige Graufarben pro Vertex
    colors = generate_random_gray_colors(len(verts))
    
    with open(output_file, 'w') as out:
        for i1, i2, i3 in faces:
            x1, y1, z1 = verts[i1]
            x2, y2, z2 = verts[i2]
            x3, y3, z3 = verts[i3]
            c1, c2, c3 = colors[i1], colors[i2], colors[i3]
            
            line = (f"{x1:.6f},{y1:.6f},{z1:.6f},{c1},"
                    f"{x2:.6f},{y2:.6f},{z2:.6f},{c2},"
                    f"{x3:.6f},{y3:.6f},{z3:.6f},{c3}")
            out.write(line + "\n")
    
    print(f"Fertig! Ergebnis geschrieben nach: {output_file}")

if __name__ == "__main__":
    main()

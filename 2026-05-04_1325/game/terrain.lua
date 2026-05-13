-- Lookup-Tabelle für Farben anhand von mapdata[x][y].x
-- Die Werte sind Beispiel-RGB-Farben und können beliebig erweitert werden.
local function rgb(r,g,b)
    return b*65536 + g*256 + r
end

local color_lookup = {
    [0] = rgb(255, 255, 255), -- Weiß
    [1] = rgb(34, 139, 34),   -- Grün
    [2] = rgb(139, 69, 19),   -- Braun
    [3] = rgb(128, 128, 128), -- Grau
    [4] = rgb(0, 0, 255),     -- Blau
    [5] = rgb(255, 255, 0),   -- Gelb
}

-- Hilfsfunktion:
-- Holt die Farbe anhand des mapdata.x Wertes.
local function get_color(tileinfo)
    local color = color_lookup[tileinfo.x]

    -- Fallback falls kein Eintrag existiert
    if not color then
        return rgb(255, 0, 255) -- Pink = Fehlerfarbe
    end

    return color
end

-- Terrain-Erzeugung
function update_terrain(terrain,xPos,yPos,zPos,heightmap, mapdata, scale, scaleH, max_distance)

    scale = scale or 1
    local player = engine.get_camera()

    engine.free_object(terrain)
    local obj = engine.create_object()

    local width = #heightmap
    local height = #heightmap[1]

    -- Wir gehen immer bis -1,
    -- weil jedes Feld mit seinem Nachbarn ein Quad bildet.
    for x = 1, width - 1 do
        for y = 1, height - 1 do

            -- Höhenwerte
            local h1 = heightmap[x][y] * scaleH
            local h2 = heightmap[x + 1][y] * scaleH
            local h3 = heightmap[x][y + 1] * scaleH
            local h4 = heightmap[x + 1][y + 1] * scaleH

            -- Positionen
            local px1 = (x - 1) * scale
            local py1 = (y - 1) * scale

            local px2 = x * scale
            local py2 = (y - 1) * scale

            local px3 = (x - 1) * scale
            local py3 = y * scale

            local px4 = x * scale
            local py4 = y * scale

            -- Farben aus mapdata holen
            local c1 = get_color(mapdata[x][y])
            local c2 = get_color(mapdata[x + 1][y])
            local c3 = get_color(mapdata[x][y + 1])
            local c4 = get_color(mapdata[x + 1][y + 1])
            
            if ((px1-player.x) > -max_distance) and ((py1-player.z) > -max_distance) and ((px1-player.x) < max_distance) and ((py1-player.z) < max_distance) then
                if ((px2-player.x) > -max_distance) and ((py2-player.z) > -max_distance) and ((px2-player.x) < max_distance) and ((py2-player.z) < max_distance) then
                    if ((px3-player.x) > -max_distance) and ((py3-player.z) > -max_distance) and ((px3-player.x) < max_distance) and ((py3-player.z) < max_distance) then
                        if ((px4-player.x) > -max_distance) and ((py4-player.z) > -max_distance) and ((px4-player.x) < max_distance) and ((py4-player.z) < max_distance) then

                            -- Erstes Dreieck
                            engine.add_triangle(
                                obj,

                                px1+xPos, h1+yPos, py1+zPos, c1,
                                px2+xPos, h2+yPos, py2+zPos, c2,
                                px3+xPos, h3+yPos, py3+zPos, c3
                            )

                            -- Zweites Dreieck
                            engine.add_triangle(
                                obj,

                                px2+xPos, h2+yPos, py2+zPos, c2,
                                px4+xPos, h4+yPos, py4+zPos, c4,
                                px3+xPos, h3+yPos, py3+zPos, c3
                            )
                            
                        end
                    end
                end
            end
            
        end
    end

    return obj
end

-- Interpoliert die Höhe zwischen den Heightmap-Punkten
-- und passt die Spielerhöhe entsprechend an.

function update_player_height(heightmap, scale, scaleH, height_offset)

    scale = scale or 1
    height_offset = height_offset or 1

    local player = engine.get_camera()

    -- Weltkoordinaten -> Heightmap-Raum
    local fx = (player.x / scale) + 1
    local fz = (player.z / scale) + 1

    local width = #heightmap
    local height = #heightmap[1]

    -- Integer-Koordinaten
    local x1 = math.floor(fx)
    local z1 = math.floor(fz)

    local x2 = x1 + 1
    local z2 = z1 + 1

    -- Prüfen ob innerhalb der Map
    if x1 < 1 or z1 < 1 or x2 > width or z2 > height then
        return
    end

    -- Lokale Position innerhalb der Zelle (0..1)
    local tx = fx - x1
    local tz = fz - z1

    -- Höhen der vier Eckpunkte
    local h11 = heightmap[x1][z1] * scaleH
    local h21 = heightmap[x2][z1] * scaleH
    local h12 = heightmap[x1][z2] * scaleH
    local h22 = heightmap[x2][z2] * scaleH

    -- Bilineare Interpolation
    local h_top = h11 * (1 - tx) + h21 * tx
    local h_bottom = h12 * (1 - tx) + h22 * tx

    local interpolated_height = h_top * (1 - tz) + h_bottom * tz

    -- Spielerhöhe setzen
    player.y = interpolated_height + height_offset

    engine.set_camera(player)
end

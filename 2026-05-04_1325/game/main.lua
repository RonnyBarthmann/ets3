dofile("game/imageLoader.lua")
dofile("game/terrain.lua")

local w, h, heightmap = loadPGM("map/heightmap.pgm")
local w, h, mapdata = loadPPM("map/mapdata512.ppm")
local terrain = -1





















--[[

-- Zufall zwischen min und max
local function rand_min_max(min, max)
    return min + (max - min) * math.random()
end

-- Zufall bis max (wie random_max)
local function random_max(max)
    return math.random(0, max)
end

-- "Chaos" generieren
local chaos_id = engine.create_object()
print(chaos_id)
if chaos_id ~= -1 then
    local chaos_x = 0
    local chaos_y = 0
    local chaos_z = 0
    local chaos_s = 5
    local chaos_i = 100

    -- Dreiecke hinzufügen
    for i = 1, chaos_i do
        engine.add_triangle(
            chaos_id,
            rand_min_max(-chaos_s, chaos_s) + chaos_x,
            rand_min_max(-chaos_s, chaos_s) + chaos_y,
            rand_min_max(-chaos_s, chaos_s) + chaos_z,
            random_max(16777215),

            rand_min_max(-chaos_s, chaos_s) + chaos_x,
            rand_min_max(-chaos_s, chaos_s) + chaos_y,
            rand_min_max(-chaos_s, chaos_s) + chaos_z,
            random_max(16777215),

            rand_min_max(-chaos_s, chaos_s) + chaos_x,
            rand_min_max(-chaos_s, chaos_s) + chaos_y,
            rand_min_max(-chaos_s, chaos_s) + chaos_z,
            random_max(16777215)
        )
    end
end

-- Woman Model laden
local woman_id = engine.load_object_from_file("models/woman")
print(woman_id)

-- Objekt verschieben
if chaos_id ~= -1 then
    engine.move_object_absolut(chaos_id, 0, 0, -10)
end
if woman_id ~= -1 then
    engine.move_object_absolut(woman_id, 0, -1, 10)
    engine.rotate_object_absolut(woman_id, 0, -90, 0)
end

]]

-- Hauptschleife
while engine.shouldQuit() == 0 do
    update_player_height(heightmap, 1, 0.1,1)
    terrain = update_terrain(terrain,0,-1,0,heightmap, mapdata, 1, 0.1,32)
    engine.step()
end

print("LUA Programm beendet.")

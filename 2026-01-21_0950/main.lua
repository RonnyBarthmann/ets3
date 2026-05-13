-- Zufall zwischen min und max
local function rand_min_max(min, max)
    return min + (max - min) * math.random()
end

-- Zufall bis max (wie random_max)
local function random_max(max)
    return math.random(0, max)
end

-- Variablen wie in C
local chaos_id = engine.create_object()
print(chaos_id)
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

-- Objekt verschieben
engine.move_object_absolut(chaos_id, 0, 0, 10)

-- Hauptschleife
while engine.shouldQuit() == 0 do
    engine.step()
end

print("LUA Programm beendet.")

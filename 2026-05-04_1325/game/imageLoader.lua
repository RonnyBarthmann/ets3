-- =========================================================
--  PGM (Grayscale) und PPM (RGB) Loader ohne externe Libs
--  Unterstützt:
--      P2 = ASCII PGM
--      P3 = ASCII PPM
--
--  Warum dieses Format?
--  -> Extrem simpel
--  -> Kein PNG/JPG Decoder nötig
--  -> Reines Textformat
--
--  Rückgabe:
--      width, height, pixels
--
--  pixels[y][x]
-- =========================================================



-- =========================================================
-- Hilfsfunktion:
-- Datei lesen
-- =========================================================
local function readFile(path)
    local file = assert(io.open(path, "r"))
    local content = file:read("*all")
    file:close()
    return content
end



-- =========================================================
-- Hilfsfunktion:
-- Kommentare entfernen (# ...)
-- =========================================================
local function stripComments(data)
    return data:gsub("#[^\n]*", "")
end



-- =========================================================
-- Hilfsfunktion:
-- Tokens zerlegen
-- =========================================================
local function tokenize(data)
    local tokens = {}

    for token in data:gmatch("%S+") do
        table.insert(tokens, token)
    end

    return tokens
end



-- =========================================================
-- GRAYSCALE LOADER
--
-- Erwartet:
--      P2 PGM Datei
--
-- Rückgabe:
--      width, height, pixels
--
-- Zugriff:
--      pixels[y][x]
--
-- Beispielwert:
--      128
-- =========================================================
function loadPGM(path)

    local data = readFile(path)
    data = stripComments(data)

    local tokens = tokenize(data)

    assert(tokens[1] == "P2", "Datei ist kein ASCII-PGM (P2)")

    local index = 2

    local width  = tonumber(tokens[index]); index = index + 1
    local height = tonumber(tokens[index]); index = index + 1

    local maxValue = tonumber(tokens[index]); index = index + 1

    local pixels = {}

    for y = 1, height do
        pixels[y] = {}

        for x = 1, width do

            local value = tonumber(tokens[index])
            index = index + 1

            -- optional auf 0..1 normalisieren:
            -- value = value / maxValue

            pixels[y][x] = value
        end
    end

    return width, height, pixels
end



-- =========================================================
-- RGB LOADER
--
-- Erwartet:
--      P3 PPM Datei
--
-- Rückgabe:
--      width, height, pixels
--
-- Zugriff:
--      pixels[y][x]
--
-- Struktur:
-- {
--      r = 255,
--      g = 128,
--      b = 64,
--
--      x = 255,
--      y = 128,
--      z = 64
-- }
-- =========================================================
function loadPPM(path)

    local data = readFile(path)
    data = stripComments(data)

    local tokens = tokenize(data)

    assert(tokens[1] == "P3", "Datei ist kein ASCII-PPM (P3)")

    local index = 2

    local width  = tonumber(tokens[index]); index = index + 1
    local height = tonumber(tokens[index]); index = index + 1

    local maxValue = tonumber(tokens[index]); index = index + 1

    local pixels = {}

    for y = 1, height do
        pixels[y] = {}

        for x = 1, width do

            local r = tonumber(tokens[index]); index = index + 1
            local g = tonumber(tokens[index]); index = index + 1
            local b = tonumber(tokens[index]); index = index + 1

            -- optional normalisieren:
            -- r = r / maxValue
            -- g = g / maxValue
            -- b = b / maxValue

            pixels[y][x] = {
                r = r,
                g = g,
                b = b,

                x = r,
                y = g,
                z = b
            }
        end
    end

    return width, height, pixels
end
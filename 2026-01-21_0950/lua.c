#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

lua_State* L;

int lua_drawObjects(lua_State* L) {
    drawObjects();
    return 0;
}

int lua_load_object_from_file(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    int id = load_object_from_file(filename);
    lua_pushinteger(L, id);
    return 1;
}

int lua_create_object(lua_State* L) {
    int id = create_object();
    lua_pushinteger(L, id);
    return 1;
}

int lua_add_triangle(lua_State* L) {
    int obj = luaL_checkinteger(L, 1);
    float x1 = luaL_checknumber(L, 2);
    float y1 = luaL_checknumber(L, 3);
    float z1 = luaL_checknumber(L, 4);
    uint32_t c1 = (uint32_t)luaL_checkinteger(L, 5);

    float x2 = luaL_checknumber(L, 6);
    float y2 = luaL_checknumber(L, 7);
    float z2 = luaL_checknumber(L, 8);
    uint32_t c2 = (uint32_t)luaL_checkinteger(L, 9);

    float x3 = luaL_checknumber(L, 10);
    float y3 = luaL_checknumber(L, 11);
    float z3 = luaL_checknumber(L, 12);
    uint32_t c3 = (uint32_t)luaL_checkinteger(L, 13);

    add_triangle(obj, x1,y1,z1,c1, x2,y2,z2,c2, x3,y3,z3,c3);
    return 0;
}

int lua_move_object(lua_State* L) {
    int id = luaL_checkinteger(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    float z = luaL_checknumber(L,4);
    move_object(id,x,y,z);
    return 0;
}

int lua_move_object_absolut(lua_State* L) {
    int id = luaL_checkinteger(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    float z = luaL_checknumber(L,4);
    move_object_absolut(id,x,y,z);
    return 0;
}

int lua_rotate_object(lua_State* L) {
    int id = luaL_checkinteger(L,1);
    float yaw = luaL_checknumber(L,2);
    float pitch = luaL_checknumber(L,3);
    float roll = luaL_checknumber(L,4);
    rotate_object(id,yaw,pitch,roll);
    return 0;
}

int lua_rotate_object_absolut(lua_State* L) {
    int id = luaL_checkinteger(L,1);
    float yaw = luaL_checknumber(L,2);
    float pitch = luaL_checknumber(L,3);
    float roll = luaL_checknumber(L,4);
    rotate_object_absolut(id,yaw,pitch,roll);
    return 0;
}

int lua_shouldQuit(lua_State* L) {
    int sq = shouldQuit();
    lua_pushinteger(L, sq);
    return 1;
}


    float speed;
    float old_time;

int lua_init(lua_State* L) {
    
    for (size_t i = 0; i<MAX_OBJECTS; i++) {
        objects[i].free = true;
    };
    
    cameraSpeed = 2;
    cameraDistance = 100;
    cameraYaw = 0;
    cameraPitch = 0;
    cameraRoll = 0;
    cameraX = 0;
    cameraY = 0;
    cameraZ = 0;
    cameraMode = RENDER_MODE_NORMAL;
    
    speed = 0;
    old_time = 0;

    framebuffer = outputInit(WINDOW_WIDTH,WINDOW_HEIGHT);
    inputInit();
    timerInit();
    
    return 0;
}

int lua_step(lua_State* L) {
    
        // clear Scene
        for (size_t i = 0; i<MAX_TRIANGLES; i++) {
            triangles[i].free = true;
        };
        clearBuffer(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,0);
        
        // Update Input
        int keyboard = updateInput();

        // Print FPS
        float time = (float)get_ms_since_start()/1000;
        float delay = time - old_time;
        old_time = time;
        printf("%f",1/delay);

        // Handle Inputs
        cameraYaw -= getMouseDeltaX() * cameraSpeed;
        cameraPitch -= getMouseDeltaY() * cameraSpeed;
        
        float yawRad = cameraYaw * (M_PI / 180.0f);
        float forward1 = sinf(yawRad);
        float forward2 = cosf(yawRad);
        
        if (keyboard&KEY_E) {
            speed = delay*5.0f;
        } else {
            speed = delay*0.5f;
        }
        if (keyboard&KEY_W) {
            cameraX -= forward1*speed;
            cameraZ += forward2*speed;
        }
        if (keyboard&KEY_S) {
            cameraX += forward1*speed;
            cameraZ -= forward2*speed;
        }
        if (keyboard&KEY_A) {
            cameraX -= forward2*speed;
            cameraZ -= forward1*speed;
        }
        if (keyboard&KEY_D) {
            cameraX += forward2*speed;
            cameraZ += forward1*speed;
        }
        if (keyboard&KEY_SHIFT) {
            cameraY += speed;
        }
        if (keyboard&KEY_Q) {
            cameraY -= speed;
        }
        if (keyboard&KEY_SPACE) {
        }
        if (keyboard&KEY_RETURN) {
        }
        if (keyboard&KEY_LEFT) {
            cameraMode = RENDER_MODE_NORMAL;
        }
        if (keyboard&KEY_RIGHT) {
            cameraMode = RENDER_MODE_WIREFRAME;
        }
        if (keyboard&KEY_UP) {
            cameraDistance += 0.1;
            printf("  %f",cameraDistance);
        }
        if (keyboard&KEY_DOWN) {
            cameraDistance -= 0.1;
            printf("  %f",cameraDistance);
        }
        if (keyboard&KEY_CTRL) {
            cameraYaw = 0;
            cameraPitch = 0;
            cameraRoll = 0;
            cameraX = 0;
            cameraY = 0;
            cameraZ = 0;
        }
        
        // Update World
//        updateChunks();
        
        // Render
        drawObjects();
        drawTriangles(framebuffer,cameraX,cameraY,cameraZ,cameraYaw,cameraPitch,cameraRoll,cameraMode);

        printf("\n",1/delay);
        framebuffer = flipBuffer();
    
    return 0;
}

lua_State* InitLUA() {
    
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_newtable(L); // engine table

    lua_pushcfunction(L, lua_drawObjects);
    lua_setfield(L,-2,"drawObjects");

    lua_pushcfunction(L, lua_load_object_from_file);
    lua_setfield(L,-2,"load_object_from_file");

    lua_pushcfunction(L, lua_create_object);
    lua_setfield(L,-2,"create_object");

    lua_pushcfunction(L, lua_add_triangle);
    lua_setfield(L,-2,"add_triangle");

    lua_pushcfunction(L, lua_move_object);
    lua_setfield(L,-2,"move_object");

    lua_pushcfunction(L, lua_move_object_absolut);
    lua_setfield(L,-2,"move_object_absolut");

    lua_pushcfunction(L, lua_rotate_object);
    lua_setfield(L,-2,"rotate_object");

    lua_pushcfunction(L, lua_rotate_object_absolut);
    lua_setfield(L,-2,"rotate_object_absolut");

    lua_pushcfunction(L, lua_shouldQuit);
    lua_setfield(L,-2,"shouldQuit");
    
    lua_pushcfunction(L, lua_init);
    lua_setfield(L,-2,"init");

    lua_pushcfunction(L, lua_step);
    lua_setfield(L,-2,"step");

    lua_setglobal(L,"default"); // Lua: engine.*
    
    return L;
}

void DoLUA(lua_State* L) {
    
    if (luaL_dofile(L,"init.lua") != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L,-1));
    }
    
    lua_close(L);
}

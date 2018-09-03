bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    context->globalScale = 3;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (u32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (u32)(context->originalWindowHeight * context->globalScale);
    context->windowTitle = "War 1";
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, NULL, NULL);
    if (!context->window)
    {
        fprintf(stderr, "GLFW window could not be created!");
        glfwTerminate();
        return false;
    }

    glfwGetWindowSize(context->window, &context->windowWidth, &context->windowHeight);
    glfwGetFramebufferSize(context->window, &context->framebufferWidth, &context->framebufferHeight);
    context->devicePixelRatio = (f32)context->framebufferWidth / (f32)context->windowWidth;

    glfwMakeContextCurrent(context->window);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        fprintf(stderr, "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return false;
    }

    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

    context->gfx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
    if (!context->gfx) 
    {
		fprintf(stderr, "Could not init nanovg.\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
		return -1;
	}

    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    context->warFilePath = ".\\Build\\DATA.WAR";
    context->warFile = loadWarFile(context->warFilePath);

    for (int i = 0; i < arrayLength(assets); ++i)
    {
        DatabaseEntry entry = assets[i];
        switch (entry.type)
        {
            case DB_ENTRY_TYPE_PALETTE:
            {
                loadPaletteResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_IMAGE:
            {
                loadImageResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_SPRITE:
            {
                loadSpriteResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_INFO:
            {
                loadLevelInfo(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_VISUAL:
            {
                loadLevelVisual(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_PASSABLE:
            {
                loadLevelPassable(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TILESET:
            {
                loadTileset(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TILES:
            {
                loadTiles(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TEXT:
            {
                loadText(context, &entry);
                break;
            }

            default:
            {
                printf("DB entries of type %d aren't handled yet\n", entry.type);
                break;
            }
        }
    }

    //createEmptyScene(context);
    createMap(context, 117);
    
    context->time = (f32)glfwGetTime();
    return true;
}

void setInputButton(WarContext* context, s32 button, bool pressed)
{
    WarInput* input = &context->input;

    input->buttons[button].wasPressed = input->buttons[button].pressed && !pressed;
    input->buttons[button].pressed = pressed;
}

void setInputKey(WarContext* context, s32 key, bool pressed)
{
    WarInput* input = &context->input;

    input->keys[key].wasPressed = input->keys[key].pressed && !pressed;
    input->keys[key].pressed = pressed;
}

void inputGame(WarContext *context)
{
    // mouse position
    f64 xpos, ypos;
    glfwGetCursorPos(context->window, &xpos, &ypos);

    vec2 pos = vec2f((f32)floor(xpos), (f32)floor(ypos));
    pos = vec2Scalef(pos, 1/context->globalScale);
    context->input.pos = pos;
    
    // mouse buttons
    setInputButton(context, WAR_MOUSE_LEFT, glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    setInputButton(context, WAR_MOUSE_RIGHT, glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    
    // keyboard keys
    setInputKey(context, WAR_KEY_CTRL, glfwGetKey(context->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                                       glfwGetKey(context->window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_SHIFT, glfwGetKey(context->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_LEFT, glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_RIGHT, glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_UP, glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_DOWN, glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS);
}

void updateViewport(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    vec2 dir = VEC2_ZERO;

    if (isKeyPressed(input, WAR_KEY_LEFT))
        dir.x = -1;
    else if (isKeyPressed(input, WAR_KEY_RIGHT))
        dir.x = 1;

    if (isKeyPressed(input, WAR_KEY_DOWN))
        dir.y = 1;
    else if (isKeyPressed(input, WAR_KEY_UP))
        dir.y = -1;

    dir = vec2Normalize(dir);

    map->viewport.x += map->scrollSpeed * dir.x * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dir.y * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);
}

void updateGame(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    updateViewport(context);

    // process all moving entities
    for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        WarEntity* entity = map->entities[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            WarTransformComponent* transform = &entity->transform;
            WarMovementComponent* movement = &entity->movement;
            if (transform->enabled && movement->enabled)
            {
                if (movement->moving)
                {
                    vec2 dir = vec2Normalize(vec2Subv(movement->target, transform->position));
                    transform->position = vec2Addv(transform->position, vec2Mulf(dir, 150 * context->deltaTime));

                    if (vec2Length(vec2Subv(movement->target, transform->position)) < 5)
                    {
                        movement->target = VEC2_ZERO;
                        movement->moving = false;
                    }
                }
            }
        }
    }

    if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the minimap panel        
        if (rectContainsf(minimapPanel, input->pos.x, input->pos.y))
        {
            vec2 minimapSize = vec2i(MINIMAP_WIDTH, MINIMAP_HEIGHT);
            vec2 offset = vec2ScreenToMinimapCoordinates(context, input->pos);

            map->viewport.x = offset.x * MAP_WIDTH / minimapSize.x;
            map->viewport.y = offset.y * MAP_HEIGHT / minimapSize.y;
        }
        // check if the click is inside the map panel
        else if(rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            if (!input->dragging)
            {
                input->dragPos = input->pos;
                input->dragging = true;
            }
        }
    }
    else if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the map panel
        if(input->dragging || rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
            pointerRect = rectScreenToMapCoordinates(context, pointerRect);

            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
                {
                    WarTransformComponent transform = entity->transform;
                    WarUnitComponent unit = entity->unit;
                    if (transform.enabled && unit.enabled)
                    {
                        rect unitRect = getUnitRect(context, entity);

                        if (rectIntersects(pointerRect, unitRect))
                        {
                            map->selectedEntities[i] = true;
                        }
                        else if (!input->keys[WAR_KEY_CTRL].pressed)
                        {
                            map->selectedEntities[i] = false;
                        }
                    }
                }
            }
        }

        input->dragging = false;
    }

    if (wasButtonPressed(input, WAR_MOUSE_RIGHT))
    {
        WarEntity* selEntity = NULL;
        
        for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
        {
            WarEntity* entity = map->entities[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_UNIT && map->selectedEntities[i])
            {
                selEntity = entity;
                break;
            }
        }

        if (selEntity)
        {
            WarUnitComponent* unit = &selEntity->unit;
            if (isDudeUnit(unit->type))
            {
                WarMovementComponent* movement = &selEntity->movement;
                if (movement->enabled)
                {
                    movement->target = vec2ScreenToMapCoordinates(context, input->pos);
                    movement->moving = true;
                }
            }
        }
    }
}

void renderScene(WarContext *context)
{
    // WarScene *scene = context->currentScene;
    // if (!scene) return;
    
    // for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    // {
    //     renderEntity(context, &scene->entities[i]);
    // }
}

void renderGame(WarContext *context)
{
    NVGcontext *gfx = context->gfx;
    
    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gfx, (f32)context->windowWidth, (f32)context->windowHeight, context->devicePixelRatio);

    renderScene(context);
    renderMap(context);

    nvgEndFrame(gfx);
}

void presentGame(WarContext *context)
{
    glfwSwapBuffers(context->window);
    glfwPollEvents();

    f32 currentTime = glfwGetTime();
    context->deltaTime = (currentTime - context->time);

    // force to 60 fps
    while (context->deltaTime <= (1.0f/60.0f))
    {
        currentTime = (f32)glfwGetTime();
        context->deltaTime = (currentTime - context->time);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);
}
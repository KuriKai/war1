void clearUIText(WarContext* context, WarEntity* entity)
{
    if (entity->text.text)
    {
        free(entity->text.text);
        entity->text.text = NULL;
    }
}

void setUIText(WarContext* context, WarEntity* entity, char* text)
{
    clearUIText(context, entity);

    if (text)
    {
        s32 length = strlen(text);
        entity->text.text = (char*)xmalloc(length + 1);
        strcpy(entity->text.text, text);
    }
}

void updateStatusText(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* txtStatus = findUIEntity(context, "txtStatus");
    assert(txtStatus);
    
    if (map->selectedEntities.count > 1)
    {
        char buffer[50];
        sprintf(buffer, "SELECTED %d UNITS", map->selectedEntities.count);
        setUIText(context, txtStatus, buffer);
    }
    else if (map->selectedEntities.count == 1)
    {
        char buffer[50];
        sprintf(buffer, "SELECTED UNIT WITH ID %d", map->selectedEntities.items[0]);
        setUIText(context, txtStatus, buffer);
    }
    else
    {
        clearUIText(context, txtStatus);
    }
}

WarEntity* createUIText(WarContext* context, char* name, vec2 position)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_TEXT);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, NULL);
    entity->text.shadowBlur = 1.0f;
    entity->text.shadowOffset = VEC2_ONE;

    return entity;
}

WarEntity* createUIImage(WarContext* context, char* name, s32 resourceIndex, vec2 position)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, resourceIndex);

    return entity;
}

WarEntity* createUIImageFromSprite(WarContext* context, char* name, s32 resourceIndex, s32 frameIndex, vec2 position)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, resourceIndex);

    entity->sprite.frameIndex = frameIndex;

    return entity;
}

void updateGoldText(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* txtGold = findUIEntity(context, "txtGold");
    assert(txtGold);

    char buffer[20];
    sprintf(buffer, "GOLD: %d", map->players[0].gold);
    setUIText(context, txtGold, buffer);
}

void updateWoodText(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* txtWood = findUIEntity(context, "txtWood");
    assert(txtWood);

    char buffer[20];
    sprintf(buffer, "LUMBER: %d", map->players[0].wood);
    setUIText(context, txtWood, buffer);
}

void updateSelectedUnitsInfo(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* imgUnitInfo = findUIEntity(context, "imgUnitInfo");
    assert(imgUnitInfo);

    WarEntity* imgUnitInfoLife = findUIEntity(context, "imgUnitInfoLife");
    assert(imgUnitInfoLife);

    if (map->selectedEntities.count >= 4)
    {
        imgUnitInfo->sprite.frameIndex = 5;
        imgUnitInfoLife->sprite.frameIndex = 8;
    }
    else if (map->selectedEntities.count >= 3)
    {
        imgUnitInfo->sprite.frameIndex = 4;
        imgUnitInfoLife->sprite.frameIndex = 7;
    }
    else if (map->selectedEntities.count >= 2)
    {
        imgUnitInfo->sprite.frameIndex = 3;
        imgUnitInfoLife->sprite.frameIndex = 6;
    }
    else if (map->selectedEntities.count >= 1)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        WarEntity* selectedEntity = findEntity(context, selectedEntityId);
        if (selectedEntity && isUnit(selectedEntity))
        {
            if (isDudeUnit(selectedEntity))
            {
                if (isMagicUnit(selectedEntity))
                    imgUnitInfo->sprite.frameIndex = 1;
                else
                    imgUnitInfo->sprite.frameIndex = 0;
            }
            else if (isBuildingUnit(selectedEntity))
            {
                imgUnitInfo->sprite.frameIndex = 2;
            }
        }

        imgUnitInfoLife->sprite.frameIndex = -1;
    }
    else
    {
        imgUnitInfo->sprite.frameIndex = -1;
        imgUnitInfoLife->sprite.frameIndex = -1;
    }
}

void renderSelectionRect(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    WarInput* input = &context->input;
    if (input->isDragging)
    {
        rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
        nvgStrokeRect(gfx, pointerRect, NVG_GREEN_SELECTION, 1.0f);
    }

    nvgRestore(gfx);
}

void renderPanels(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity *entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_IMAGE)
        {
            renderEntity(context, entity, false);
        }
    }

    nvgRestore(gfx);
}

void renderMinimap(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    // copy the minimap base to the first frame which is the one that will be rendered
    memcpy(map->minimapSprite.frames[0].data, map->minimapSprite.frames[1].data, MINIMAP_WIDTH * MINIMAP_HEIGHT * 4);

    for(s32 i = 0; i < map->entities.count; i++)
    {
        u8 r = 211, g = 211, b = 211;

        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            WarUnitComponent unit = entity->unit;

            if (unit.type == WAR_UNIT_TOWNHALL_HUMANS || 
                unit.type == WAR_UNIT_TOWNHALL_ORCS)
            {
                r = 255; g = 255; b = 0;
            }
            else if(unit.player == 0)
            {
                r = 0; g = 199; b = 0;
            }
            else if(unit.player < 4)
            {
                r = 199; g = 0; b = 0;
            }

            WarTransformComponent transform = entity->transform;
            s32 tileX = (s32)(transform.position.x/MEGA_TILE_WIDTH);
            s32 tileY = (s32)(transform.position.y/MEGA_TILE_HEIGHT);

            for(s32 y = 0; y < unit.sizey; y++)
            {
                for(s32 x = 0; x < unit.sizex; x++)
                {
                    s32 pixel = (tileY + y) * MINIMAP_WIDTH + (tileX + x);
                    map->minimapSprite.frames[0].data[pixel * 4 + 0] = r;
                    map->minimapSprite.frames[0].data[pixel * 4 + 1] = g;
                    map->minimapSprite.frames[0].data[pixel * 4 + 2] = b;
                }
            }
        }
    }

    nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);

    updateSpriteImage(context, &map->minimapSprite, map->minimapSprite.frames[0].data);
    renderSprite(context, &map->minimapSprite, VEC2_ZERO, VEC2_ONE);

    nvgRestore(gfx);
}

void renderMinimapViewport(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);
    nvgTranslate(gfx, map->viewport.x * MINIMAP_MAP_WIDTH_RATIO, map->viewport.y * MINIMAP_MAP_HEIGHT_RATIO);

    nvgStrokeRect(gfx, recti(0, 0, MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT), NVG_WHITE, 1.0f/context->globalScale);

    nvgRestore(gfx);
}

void renderMapUI(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    // render selection rect
    renderSelectionRect(context);

    // render images
    renderPanels(context);
    
    // render minimap
    renderMinimap(context);
    renderMinimapViewport(context);

    // render text
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity *entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_TEXT)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // DEBUG:
    // Render debug info
    {
        char debugText[256];

        sprintf(debugText, 
            "Debug info:\n"
            "speed: %.2f\n"
            "scale: %.2f\n"
            "editing trees = %d\n"
            "editing roads = %d\n"
            "editing walls = %d\n"
            "editing ruins = %d\n",
            context->globalSpeed,
            context->globalScale,
            context->editingTrees,
            context->editingRoads,
            context->editingWalls,
            context->editingRuins);

        rect r = recti(map->mapPanel.x + 2, map->mapPanel.y + 2, 50, 50);
        nvgFillRect(gfx, r, nvgRGBA(50, 50, 50, 200));

        NVGfontParams params = nvgCreateFontParams("defaultFont", 5.0f, nvgRGBA(200, 200, 200, 255));
        nvgMultilineText(gfx, debugText, r.x, r.y, r.width, r.height, params);
    }

    nvgRestore(gfx);
}
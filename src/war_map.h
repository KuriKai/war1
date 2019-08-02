#pragma once

void addEntityToSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;

    // subtitute this with a set data structure that doesn't allow duplicates
    if (!WarEntityIdListContains(&map->selectedEntities, id))
        WarEntityIdListAdd(&map->selectedEntities, id);
}

void removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    WarEntityIdListRemove(&map->selectedEntities, id);
}

void clearSelection(WarContext* context)
{
    WarMap* map = context->map;
    WarEntityIdListClear(&map->selectedEntities);
}

WarEntityList* getEntities(WarMap* map)
{
    return &map->entities;
}

WarEntityList* getEntitiesOfType(WarMap* map, WarEntityType type)
{
    return WarEntityMapGet(&map->entitiesByType, type);
}

WarEntityList* getUnitsOfType(WarMap* map, WarUnitType type)
{
    return WarUnitMapGet(&map->unitsByType, type);
}

WarEntityList* getUIEntities(WarMap* map)
{
    return &map->uiEntities;
}

vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    v = vec2Translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2Translatef(v, viewport.x, viewport.y);
    return v;
}

vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->minimapPanel;
    
    v = vec2Translatef(v, -minimapPanel.x, -minimapPanel.y);
    return v;
}

rect rectScreenToMapCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    r = rectTranslatef(r, -mapPanel.x, -mapPanel.y);
    r = rectTranslatef(r, viewport.x, viewport.y);
    return r;
}

vec2 vec2MapToScreenCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    v = vec2Translatef(v, -map->viewport.x, -map->viewport.y);
    v = vec2Translatef(v, map->mapPanel.x, map->mapPanel.y);
    return v;
}

rect rectMapToScreenCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;

    r = rectTranslatef(r, -map->viewport.x, -map->viewport.y);
    r = rectTranslatef(r, map->mapPanel.x, map->mapPanel.y);
    return r;
}

vec2 vec2MapToTileCoordinates(vec2 v)
{
    v.x = floorf(v.x / MEGA_TILE_WIDTH);
    v.y = floorf(v.y / MEGA_TILE_HEIGHT);
    return v;
}

vec2 vec2TileToMapCoordinates(vec2 v, bool centeredInTile)
{
    v.x *= MEGA_TILE_WIDTH;
    v.y *= MEGA_TILE_HEIGHT;

    if (centeredInTile)
    {
        v.x += halfi(MEGA_TILE_WIDTH);
        v.y += halfi(MEGA_TILE_HEIGHT);
    }

    return v;
}

vec2 vec2MinimapToViewportCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->minimapPanel;
    vec2 minimapPanelSize = vec2f(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2f(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);

    v = vec2Translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2Clampv(v, VEC2_ZERO, vec2Subv(minimapPanelSize, minimapViewportSize));
    return v;
}

u8Color getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
void updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
s32 getMapTileIndex(WarContext* context, s32 x, s32 y);
void setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile);

WarMapTile* getMapTileState(WarMap* map, s32 x, s32 y);
void setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState);
void setUnitMapTileState(WarMap* map, WarEntity* entity, WarMapTileState tileState);
bool checkMapTiles(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, s32 states);
bool checkUnitTiles(WarMap* map, WarEntity* entity, s32 states);

#define isTileUnkown(map, x, y) ((map)->tiles[(y) * MAP_TILES_WIDTH + (x)].state == MAP_TILE_STATE_UNKOWN)
#define isTileFog(map, x, y) ((map)->tiles[(y) * MAP_TILES_WIDTH + (x)].state == MAP_TILE_STATE_FOG)
#define isTileVisible(map, x, y) ((map)->tiles[(y) * MAP_TILES_WIDTH + (x)].state == MAP_TILE_STATE_VISIBLE)

void changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);

void handleOpenMenu(WarContext* context, WarEntity* entity);
void handleContinue(WarContext* context, WarEntity* entity);
void handleOpenOptions(WarContext* context, WarEntity* entity);
void handleOptionsOk(WarContext* context, WarEntity* entity);
void handleOptionsCancel(WarContext* context, WarEntity* entity);
void handleGameOverContinue(WarContext* context, WarEntity* entity);
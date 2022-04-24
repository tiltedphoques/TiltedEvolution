#pragma once

/**
* @brief Dispatched when the local player changes location.
* 
* In-game locations are different from cells. Unlike cells,
* locations are usually marked on the world map.
*/
struct LocationChangeEvent
{
    LocationChangeEvent(){};
};

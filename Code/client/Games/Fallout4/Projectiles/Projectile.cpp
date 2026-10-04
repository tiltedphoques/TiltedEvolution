#include <Projectiles/Projectile.h>

BSPointerHandle<Projectile>* Projectile::Launch(BSPointerHandle<Projectile>* apResult, LaunchData&) noexcept
{
    // Fallout 4 ProjectileLaunchData differs from Skyrim's; remote launches are not implemented yet.
    *apResult = {};
    return apResult;
}

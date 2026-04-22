# IN2026 Asteroids Coursework Design Document

## Part I (Implemented option **b**: asteroid interactions)

### 1) Features implemented
- ✅ Asteroids now bounce off each other; both direction and speed are changed on collision.
- ✅ Large asteroids are destroyed when colliding with the spaceship (and the spaceship is also destroyed unless invulnerable).
- ✅ Large asteroids split into two small asteroids when hit by a bullet.
- ✅ Small asteroids are destroyed when hit by a bullet.
- ✅ Small asteroids bounce off other asteroids and also bounce with the spaceship.
- ✅ Score values differ by asteroid size:
  - Large asteroid destroyed by bullet: +20
  - Small asteroid destroyed by bullet: +50
- ✅ No score is awarded when the spaceship collides with an asteroid.
- ❌ Start menu/high-score screen path (Part I option a) was not implemented.

### 2) Methods/classes changed
- `SRC/Asteroid.h`, `SRC/Asteroid.cpp`
  - Added asteroid size model (`LARGE`, `SMALL`)
  - Added bullet-destruction tracking and size-based scoring
  - Added collision response bounce logic
- `SRC/Asteroids.h`, `SRC/Asteroids.cpp`
  - Added splitting logic when large asteroid is shot
  - Added helper to create small asteroids
- `SRC/ScoreKeeper.h`
  - Changed scoring to award points only for bullet-destroyed asteroids, using size-specific values
- `VC/Asteroids/Asteroids.vcxproj`
  - Added new source/header entries for power-up implementation files

### 3) Implementation notes with snippets
- **Bounce response** uses a normal/tangent decomposition to exchange normal velocity components:
  ```cpp
  GLfloat this_normal_speed = this_velocity.dot(delta);
  GLfloat other_normal_speed = other_velocity.dot(delta);
  GLVector3f this_new_velocity = (other_normal + this_tangent) * 0.95f;
  GLVector3f other_new_velocity = (this_normal + other_tangent) * 0.95f;
  ```
- **Split on bullet hit** is implemented by detecting large asteroid removal triggered by bullet and spawning two small asteroids at the impact point:
  ```cpp
  if (asteroid->GetSize() == Asteroid::LARGE && asteroid->WasDestroyedByBullet()) {
      CreateSmallAsteroids(object->GetPosition(), object->GetVelocity());
  }
  ```
- **No score on ship collision** is enforced by scoring only if asteroid carries the `WasDestroyedByBullet()` flag.

### 4) In-game screenshots
- Add screenshots here from your run/build environment:
  - Part I asteroid bounce
  - Part I asteroid split + score update

---

## Part II (3 bonuses/power-ups implemented)

### 1) Features implemented
- ✅ **Extra life** collectible.
- ✅ **Invulnerability** for a limited time (8 seconds).
- ✅ **Spaceship control upgrade (brakes)** for a limited time (12 seconds), activated with down-arrow.

### 2) Methods/classes changed
- New class:
  - `SRC/PowerUp.h`, `SRC/PowerUp.cpp`
    - Collectible entity with type enum and time-to-live
- `SRC/Asteroids.h`, `SRC/Asteroids.cpp`
  - Added timed power-up spawning
  - Added application of effects when collected
- `SRC/Spaceship.h`, `SRC/Spaceship.cpp`
  - Added invulnerability timer/state
  - Added brakes timer/state and `ApplyBrake()`
- `SRC/Player.h`
  - Added `AddLife()` and `GetLives()`

### 3) Implementation notes with snippets
- **Power-up spawn loop** uses session timers:
  ```cpp
  if (value == SPAWN_POWERUP) {
      SpawnPowerUp();
      SetTimer(12000, SPAWN_POWERUP);
  }
  ```
- **Collection behaviour** is implemented in `PowerUp::OnCollision`, then applied in `Asteroids::OnObjectRemoved`:
  ```cpp
  if (powerup->GetPowerUpType() == PowerUp::EXTRA_LIFE) { mPlayer.AddLife(); }
  else if (powerup->GetPowerUpType() == PowerUp::INVULNERABILITY) { mSpaceship->SetInvulnerableFor(8000); }
  else if (powerup->GetPowerUpType() == PowerUp::BRAKES) { mSpaceship->EnableBrakesFor(12000); }
  ```
- **Braking control upgrade**:
  ```cpp
  case GLUT_KEY_DOWN: mSpaceship->ApplyBrake(); break;
  ```

### 4) In-game screenshots
- Add screenshots here from your run/build environment:
  - Extra life pickup updating lives label
  - Invulnerability preventing ship death
  - Brakes reducing ship velocity

#include "marky_camera.h"

#include "common.h"
#include "string_hash.h"

#include "camera.h"

VALIDATE_SIZE(marky_camera, 0x1dcu);



void * marky_camera::operator new(size_t size)
{
    return _aligned_malloc(size, 4);
}



void marky_camera::sync(camera &a2) {
    if ( !this->is_externally_controlled() ) {
        game_camera::sync(a2);
    }
}

void marky_camera::camera_set_collide_with_world(bool a2) {
    this->field_1BC = a2;
}

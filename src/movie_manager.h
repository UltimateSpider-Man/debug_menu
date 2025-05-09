#pragma once

struct movie_manager {
    movie_manager();

    static bool load_and_play_movie(const char *a2, const char *str, bool a3);

    void movie_play_handler_patch();
};
void movie_manager_patch();
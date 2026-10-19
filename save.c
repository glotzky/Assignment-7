#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"


/*
Saves server game data to specified filepath.
Returns 0 on success, -1 on failure.
*/
int savegame(struct gameDB* game, char* filepath) {
    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        return -1;
    }

    ssize_t bytes_written = write(fd, game, sizeof(*game));
    int close_result = close(fd);
    if (bytes_written != sizeof(*game) || close_result < 0) {
        return -1;
    }
    return 0;
}

/*
Loads a file written by savegame into game.
Returns 0 on success, -1 on failure.
*/
int loadgame(struct gameDB* game, char* filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    ssize_t bytes_read = read(fd, game, sizeof(*game));
    int close_result = close(fd);
    if (bytes_read != sizeof(*game) || close_result < 0) {
        return -1;
    }
    return 0;
}
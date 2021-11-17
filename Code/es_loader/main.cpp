#include "ESLoader.h"

int main(int argc, char** argv)
{
    ESLoader loader("game_files");
    loader.BuildFileList();
}


#pragma once
#include "Minecraft.World/level/biome/Biome.h"

class ExtremeHillsBiome : public Biome {
public:
    enum EType {
        DEFAULT = 0,
        EDGE = 1,
        M = 2
    };

    ExtremeHillsBiome(EBiomeIDs id, EType type, BiomeProperties *properties);

    int getBaseClass() override;

    char padding[0x10];
};

static_assert(sizeof(ExtremeHillsBiome) == 0x158);

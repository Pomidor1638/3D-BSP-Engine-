#include "Core.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>

#ifdef  SAFE_VERSION
GLuint texture[4];
#else
GLuint Texture[150];
#endif

void ShowVec3(vec3_t p) {
    for (int i = 0; i < 3; i++)
        std::cout << p[i] << ' ';
    std::cout << std::endl;
}


void LoadTexturesToGPU() {
    int width, height, cnt, i, j;
    std::string filename;
    unsigned char* data;
    glGenTextures(sizeof(Texture) / sizeof(GLuint), (GLuint*)(&Texture));
    int numtextures = 0;

    struct {
        std::string name;
        int count;
        std::string format;
    } textures[11] = {
        {"chess-",                         1, ".bmp"},
        {"joebidon-",                      1, ".bmp"},
        {"luxa.org-dithered-brick-",      11, ".bmp"},
        {"luxa.org-dithered-concrete-",   20, ".bmp"},
        {"luxa.org-dithered-dagestan-",   10, ".bmp"},
        {"luxa.org-dithered-grass-",      10, ".bmp"},
        {"luxa.org-dithered-ice-",        10, ".bmp"},
        {"luxa.org-dithered-marble-",     40, ".bmp"},
        {"luxa.org-dithered-metal-",      10, ".bmp"},
        {"luxa.org-dithered-road-brick-", 20, ".bmp"},
        {"luxa.org-dithered-water-",      10, ".bmp"},
    };

    for (i = 0; i < sizeof(textures)/sizeof(textures[0]); i++) {

        for (j = 1; j <= textures[i].count; j++) {
            filename = "TEXTURES/" + textures[i].name + std::to_string(j) + textures[i].format;
            printf("%s, %i\n", filename.c_str(), numtextures + 1);
            data = stbi_load(filename.c_str(), &width, &height, &cnt, 0);
            glBindTexture(GL_TEXTURE_2D, Texture[numtextures++]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, (cnt == 4) ? (GL_RGBA) : (GL_RGB), width, height,
                0, (cnt == 4) ? (GL_RGBA) : (GL_RGB), GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }


	glBindTexture(GL_TEXTURE_2D, 0);

}

#define HEADER_LUMPS 13
#define BSPVERSION 130

enum LUMP_SHIT {
    LUMP_PLANES,
    LUMP_VERTEXES,
    LUMP_VTABLE,
    LUMP_PORTALS,
    LUMP_PORTALTABLE,
    LUMP_FACES,
    LUMP_NODES,
    LUMP_LEAFS,
    LUMP_CLIPNODES,
    LUMP_TEXINFO,
    LUMP_LIGHTING,
    LUMP_ENTITIES,
    LUMP_MODELS,
};



bool LoadBSPtree(std::string filename, bspmap_t* map) {
    std::ifstream file(filename, std::ios::binary); 
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    if (!file) {
        std::cerr << "Failed to open file\n" << filename << std::endl;
        return false;
    }
    std::cout << filename << " is loading" << std::endl;

    struct LumpInfo {
        int lumpType;
        void** targetPointer;
        size_t elementSize;
        int* elementCount;
        const char* name;
    };

    struct header_s {
        int version;
        struct lump_s {
            int fileofs, filelen;
        } lumps[HEADER_LUMPS];
    } header;

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&header), sizeof(header_s));

    if (header.version != BSPVERSION) {
        file.close();
        return false;
    }

    memset(map, 0, sizeof(bspmap_t));

    std::vector<LumpInfo> lumpsToLoad = {
        { LUMP_PLANES     , reinterpret_cast<void**>(&map->dplanes     ), sizeof(plane_s       ), &map->numplanes     ,"dplanes"      },
        { LUMP_VERTEXES   , reinterpret_cast<void**>(&map->dvertexes   ), sizeof(vec3_t        ), &map->numvert       ,"dvertexes"    },
        { LUMP_VTABLE     , reinterpret_cast<void**>(&map->dvertextable), sizeof(unsigned int  ), &map->numvertextable,"dvertextable" },
        { LUMP_PORTALS    , reinterpret_cast<void**>(&map->dportals    ), sizeof(dportal_t     ), &map->numportals    ,"dportals"     },
        { LUMP_PORTALTABLE, reinterpret_cast<void**>(&map->dportaltable), sizeof(unsigned short), &map->numportaltable,"dportaltable" },
        { LUMP_FACES      , reinterpret_cast<void**>(&map->dfaces      ), sizeof(dface_t       ), &map->numfaces      ,"dfaces"       },
        { LUMP_NODES      , reinterpret_cast<void**>(&map->dnodes      ), sizeof(dnode_t       ), &map->numnodes      ,"dnodes"       },
        { LUMP_LEAFS      , reinterpret_cast<void**>(&map->dleafs      ), sizeof(dleaf_t       ), &map->numleafs      ,"dleafs"       },
        { LUMP_CLIPNODES  , reinterpret_cast<void**>(&map->dclipnodes  ), sizeof(dclipnode_t   ), &map->numclipnodes  ,"dclipnodes"   },
        { LUMP_TEXINFO    , reinterpret_cast<void**>(&map->texinfo     ), sizeof(dtexinfo_t    ), &map->numdtexinfo   ,"texinfo"      },
        { LUMP_LIGHTING   , reinterpret_cast<void**>(&map->dlightdata  ), sizeof(byte          ), &map->lightdatasize ,"dlightdata"   },
        { LUMP_MODELS     , reinterpret_cast<void**>(&map->dmodels     ), sizeof(dmodel_t      ), &map->nummodels     ,"dmodels"      }
    };

    for (const auto& lumpInfo : lumpsToLoad) {
        header_s::lump_s* lump = &header.lumps[lumpInfo.lumpType];
        file.seekg(lump->fileofs, std::ios::beg);
        *lumpInfo.elementCount = lump->filelen / lumpInfo.elementSize;
        *lumpInfo.targetPointer = new char[lump->filelen]; 
        file.read(reinterpret_cast<char*>(*lumpInfo.targetPointer), lump->filelen);
        printf("%6i %14s %7i\n", *lumpInfo.elementCount, lumpInfo.name, lump->filelen);
    }

    file.close();
    return true;
}



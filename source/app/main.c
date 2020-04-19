#include "./main.h"
#include <stdio.h>
#include "tonc.h"
#include "audio.h"
#include "gbfs.h"

// Audio createEmptyAudio() {
//     Audio audio;
//     audio.numChannels = 0;
//     return audio;
// }

// Audio loadAudio() {
//     u32 audioDataSize = 0;
//     const GBFS_FILE * audioFile = find_first_gbfs_file(find_first_gbfs_file);
//     const u16 * audioData = gbfs_get_obj(audioFile, "Test00.bin", &audioDataSize);

//     if (audioData == NULL) {
//         tte_printf("No GBFS audio found.\n");
//         return createEmptyAudio();
//     }

//     return loadAudioFromROM(audioData);
// }

void setupGBA() {
    // set Display mode
    REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

    // setup interrupts
    irq_init(NULL);

    // setupTextMode
    tte_init_se_default(3, BG_CBB(0) | BG_SBB(31));
    tte_init_con();
}


const int L1x = 4, L1y = 16;
const int L2x = 0, L2y = 7;
const int L3x = 8, L3y = 9;
const int L4x = 4, L4y = 0;

void drawLighting(int x, int y) {
    m3_line(x + L1x, y + L1y,   x + L2x, y + L2y, CLR_YELLOW);
    m3_line(x + L2x, y + L2y,   x + L3x, y + L3y, CLR_YELLOW);
    m3_line(x + L3x, y + L3y,   x + L4x, y + L4y, CLR_YELLOW);
}
typedef struct Level {
    Energy energies;
    Gate gates;
} Level;

typedef struct Energy {
    u8 index;
    u8 x, y;
    u8 pos[10];
} Energy;

typedef struct Gate {
    u8 x, y;
    bool horizontal;
} Gate;


Energy e = (Energy){.index=0, .x=20, .y=20, .pos={220,20, 220,80, 20,80} };
Gate g = (Gate){.x=120, .y=80, .horizontal=true};

bool playLevel(u8 level) {

    int CLR_BACKGROUND = RGB8(31, 119, 77);
    int CLR_LINE = RGB8(155, 199, 84);
    int CLR_ENERGY = RGB8(50, 50, 180);
    int CLR_GATE = RGB8(50, 50, 180);

    bool flipState = true;

    int frame=0;
    int inputFrame=0;
    while (true) {
        vid_vsync();

        key_poll();

        if (key_tri_horz() != 0 && frame - inputFrame > 10) {
            inputFrame = frame;
            flipState = !flipState;
        }

        m3_fill(CLR_BACKGROUND);

        m3_frame(20, 20, 240-20, 160-20, CLR_LINE);

//        m3_vline(100, 20, 160-20, CLR_LINE);
//        m3_vline(140, 20, 160-20, CLR_LINE);
        m3_hline(20, 80, 240-20, CLR_LINE); // hline x1, y, x2


        // gates
        m3_rect(g.x-5,g.y-5, g.x+5,g.y+5, CLR_BACKGROUND);
        if (g.horizontal == flipState) { m3_rect(g.x-5,g.y-1, g.x+5,g.y+1, CLR_ENERGY); }
        else                           { m3_rect(g.x-1,g.y-5, g.x+1,g.y+5, CLR_ENERGY); }


        // Energies
        m3_rect(e.x-2,e.y-2, e.x+2,e.y+2, CLR_ENERGY);

        if (e.x == e.pos[e.index*2] && e.y == e.pos[e.index*2+1]) {e.index += 1;}
        e.x = (e.x < e.pos[e.index*2  ])? ++e.x : ((e.x == e.pos[e.index*2  ])? e.x : --e.x);
        e.y = (e.y < e.pos[e.index*2+1])? ++e.y : ((e.y == e.pos[e.index*2+1])? e.y : --e.y);

        frame++;
    }

    return true;
}

int main() {

//    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
//    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
//
//    tte_write("#{P:62,64}");
//    tte_write("Save the GameBoy!");
//
//    tte_write("#{P:10,140}");
//    tte_write("Press the <any> key to start");
//
//    while (!KEY_DOWN_NOW(KEY_ANY));


    setupGBA();
    int level = 1;
    while (true) {
        bool hasWon = playLevel(level);
        if (hasWon == false) {
            break;
        }
    }

//    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
//    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
//    vid_vsync();
//
//    tte_write("#{P:62,64}");
//    tte_write("You Lose!");



//    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
//    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
//    vid_vsync();
//
//    tte_write("#{P:62,64}");
//    tte_write("You win!");

    while(1);
    return 0;
}

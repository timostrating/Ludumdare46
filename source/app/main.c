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



typedef struct Line {
    u8 x1, y1;
    u8 x2, y2;
} Line;

typedef struct Energy {
    u8 index;
    u8 x, y;
    u8 pos[10];
} Energy;

typedef struct Gate {
    u8 x, y;
    bool horizontal;
    bool done;
} Gate;

typedef struct Level {
    u8 nrLs; Line ls[12];
    u8 nrGs; Gate gs[12];
    u8 nrEs; Energy es[12];
} Level;


#define W 24

#define LINE(a,b, c,d) (Line){.x1=(a)*10, .y1=(b)*10, .x2=(c)*10, .y2=(d)*10}
#define GATE(a,b, c) (Gate){.x=(a)*10, .y=(b)*10, .horizontal=c, .done=false}
#define ENERGY(a,b, x1,x2, x3,x4, x5,x6, x7,x8, x9,x10, x11,x12) (Energy){.index=0, .x=(a)*10, .y=(b)*10, .pos={(x1)*10,(x2)*10, (x3)*10,(x4)*10, (x5)*10,(x6)*10, (x7)*10,(x8)*10, (x9)*10,(x10)*10, (x11)*10,(x12)*10} }
#define A 2
#define B 4
#define C 6
#define D 8
#define E 10
#define F 12
#define G 14
#define H 16
#define I 18
#define J 20
#define K 22


Level levels[] = {
    { // level 1
        .nrLs=1, .ls={ LINE(A, 8, K, 8), },
        .nrGs=1, .gs={ GATE(F, 8, false) },
        .nrEs=2, .es={
            ENERGY(K,2, A,2, A,14, K,14, K,2, A,2, 0,0),
            ENERGY(A,2, K,2, K,8,  A,8,  0,0, 0,0, 0,0),
        },
    },
    { // level 2
        .nrLs=3, .ls={
            LINE(A, 8, K, 8),
            LINE(F-3, 2, F-3, 14),
            LINE(F+3, 2, F+3, 14),
        },
        .nrGs=3, .gs={
            GATE(F, 8, true),
            GATE(F-3, 8-2, true),
            GATE(F+3, 8+2, true),
        },
        .nrEs=5, .es={
            ENERGY(K,12, K,2, A,2, A,8, K,8, K,2, A,2), // kills 1
            ENERGY(A,4, A,2, F-3,2, F-3,14, 0,0, 0,0, 0,0), // kills 2
            ENERGY(K,8, K,14, K,14, F+3,14, F+3,2, 0,0, 0,0), // kills 3
            ENERGY(A,9, A,2, A,8, K,8, K,2, A,14, 0,0),
            ENERGY(F,2, K,2, K,14, A,14, A,2, K,2, 0,0),
        },
    },
    { // level 3
        .nrLs=4, .ls={
            LINE(A, 6, K, 6),
            LINE(A, 10, K, 10),
            LINE(F-3, 2, F-3, 14),
            LINE(F+3, 2, F+3, 14),
        },
        .nrGs=4, .gs={
            GATE(F-3, 6, true),
            GATE(F-3, 10, false),
            GATE(F+3, 6, false),
            GATE(F+3, 10, true),
        },
        .nrEs=4, .es={
            ENERGY(F,2, K,2, K,14, A,14, A,2, K,2, 0,0),
            ENERGY(I,2, A,2, A,6,  K,6, K,12, K,2, A,2),
            ENERGY(F,14, F+3,14, F+3,2, F-3,2, F-3,12, A,12, A,2), // kills 1 2 3 4
            ENERGY(A,12, A,2, K,2, K,14, A,14, A,2, K,2),
        },
    },
    { // level 4
        .nrLs=8, .ls={
            LINE(A, 6, I, 6),
            LINE(I, 6, K, 2),
            LINE(A, 10, I, 10),
            LINE(I, 10, K, 6),
            LINE(K, 12, G, 12),
            LINE(G, 12, G, 2),
            LINE(E, 2, E, 14),
            LINE(F-5, 2, F-5, 14),
        },
        .nrGs=6, .gs={
            GATE(F-2, 6, false),
            GATE(F-2, 10, true),
            GATE(F-5, 6, false),
            GATE(F-5, 10, true),
            GATE(F+2,   6, true),
            GATE(F+2,   10, false),
        },
        .nrEs=4, .es={
            ENERGY(K,9, K,2, I,6, A,6, A,2, K,2, K,14),
            ENERGY(B,14, F-2,14, F-2,2, A,2, A,14, K,14, K,2),
            ENERGY(A,13, A,2, A,10, I,10, K,6, K,14, A,14),
            ENERGY(A,6, A,2, K,2, K,14, A,14, A,2, K,2),
            ENERGY(I,14, K,14, K,2, I,10, A,10, A,2, K,2),
        },
    },
};

bool playLevel(u8 lvl) {

    int CLR_BACKGROUND = RGB8(31, 119, 77);
    int CLR_LINE = RGB8(155, 199, 84);
    int CLR_ENERGY = RGB8(50, 50, 180);
    int CLR_GATE = RGB8(50, 50, 180);
    int CLR_GATE_DONE = RGB8(50, 180, 50);

    bool horizontalState = true;
    Level level = levels[lvl];

    int frame=0;
    int inputFrame=0;
    int gates = level.nrGs;

    while (true) {
        vid_vsync();

        key_poll();

        if (key_tri_vert() != 0 && frame - inputFrame > 10) {
            return true;
        }

        if (key_tri_horz() != 0 && frame - inputFrame > 10) {
            inputFrame = frame;
            horizontalState = !horizontalState;
        }

        m3_fill(CLR_BACKGROUND);

        m3_frame(20, 20, 240-20, 160-20, CLR_LINE);

        // Lines
        Line* l = &(level.ls[0]);
        for (int i=0; i<level.nrLs; i++) {
            l = &(level.ls[i]);
            m3_line(l->x1, l->y1, l->x2, l->y2, CLR_LINE);
        }


        // Gates
        Gate* g = &(level.gs[0]);
        for (int i=0; i<level.nrGs; i++) {
            g = &(level.gs[i]);
            if (!g->done)m3_rect(g->x-5,g->y-5, g->x+5,g->y+5, CLR_BACKGROUND);
            if (g->horizontal == horizontalState) { m3_rect(g->x-5,g->y-1, g->x+5,g->y+1, (g->done)? CLR_GATE_DONE : CLR_GATE); }
            else                                  { m3_rect(g->x-1,g->y-5, g->x+1,g->y+5, (g->done)? CLR_GATE_DONE : CLR_GATE); }
        }


        // Energies
        Energy* e = &(level.es[0]);
        for (int i=0; i<level.nrEs; i++) {
            e = &(level.es[i]);

            m3_rect(e->x-2,e->y-2, e->x+2,e->y+2, CLR_ENERGY);
            m3_rect(e->x-1,e->y-1, e->x+1,e->y+1, CLR_ENERGY + (e->x % 7) * 5);

            if (e->x == e->pos[e->index*2] && e->y == e->pos[e->index*2+1]) {e->index += 1;}
            e->x = (e->x < e->pos[e->index*2  ])? ++e->x : ((e->x == e->pos[e->index*2  ])? e->x : --e->x);
            e->y = (e->y < e->pos[e->index*2+1])? ++e->y : ((e->y == e->pos[e->index*2+1])? e->y : --e->y);

            for (int i=0; i<level.nrGs; i++) {
                g = &(level.gs[i]);
                if (!g->done && e->x == g->x && e->y == g->y) {
                    if ((g->horizontal == horizontalState) != (e->y == e->pos[e->index * 2 + 1])) { return false; }
                    else {
                        g->done = true;
//                        drawLighting(g->x, g->y);
//                        vid_vsync();
                        if (--gates <= 0) { return true; }
                    }
                }
            }
        }

        frame++;
    }

    return true;
}

void youWin() {
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
    vid_vsync();

    tte_write("#{P:62,64}");
    tte_write("You win!");

    while (!KEY_DOWN_NOW(KEY_ANY));
    main();
}

void youLose() {
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
    vid_vsync();

    tte_write("#{P:72,64}");
    tte_write("You Lose!");

    while (!KEY_DOWN_NOW(KEY_ANY));
    main();
}

bool gameLoop() {
    setupGBA();
    int level = 0;
    int highest = 0;
    while (true) {
        setupGBA();
        bool hasWon = playLevel(level);

        if (level == 5) { return true; }
        if (hasWon == false) {
            if (highest > level)
                return false;
            level--;
        } else {
            level++;
            if (highest < level)
                highest = level;
        }
    }
}

int main() {

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));

    tte_write("#{P:62,64}");
    tte_write("Save the GameBoy!");

    tte_write("#{P:10,140}");
    tte_write("Press the <any> key to start");

    while (!KEY_DOWN_NOW(KEY_ANY));

    bool game = gameLoop();
    if (game) youWin();
    else youLose();

    while(1);
    return 0;
}

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


int main()
{

    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));

    tte_write("#{P:62,64}");
    tte_write("Save the GameBoy!");

    tte_write("#{P:10,140}");
    tte_write("Press the <any> key to start");

    while(!KEY_DOWN_NOW(KEY_ANY));


    int ii, jj;
    setupGBA();

    // Fill screen with grey color
    m3_fill(CLR_GRAY);

    // Lines in bottom left frame
    for(ii=0; ii<=10; ii++)
    {
        jj= 3*ii+7;
        m3_line(24*ii, 0, 240-24*ii, 160, RGB15(0, jj, jj));
    }

    while(1);
    return 0;
}

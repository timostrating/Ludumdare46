#include "./main.h"
#include <stdio.h>
#include "tonc.h"
#include "audio.h"
#include "gbfs.h"

Audio createEmptyAudio() {
    Audio audio;
    audio.numChannels = 0;
    return audio;
}

Audio loadAudio() {
    u32 audioDataSize = 0;
    const GBFS_FILE * audioFile = find_first_gbfs_file(find_first_gbfs_file);
    const u16 * audioData = gbfs_get_obj(audioFile, "Test00.bin", &audioDataSize);

    if (audioData == NULL) {
        tte_printf("No GBFS audio found.\n");
        return createEmptyAudio();
    }

    return loadAudioFromROM(audioData);
}

int main()
{
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));
    tte_init_con();

    initAudioSystem();
    irq_init(NULL);
    irq_add(II_VBLANK, tickAudioSystem);

    tte_printf("Hello, World!\n\n");

    int result = foo();
    result += bar(3, 2);
    tte_printf("The result = %d.\n", result);

    tte_printf("Now playing audio.\n");
    Audio audio = loadAudio();
    setCurrentAudio(&audio);

    tte_printf("\nKind regards,\nLuc van den Brand.");

    while(1);
}

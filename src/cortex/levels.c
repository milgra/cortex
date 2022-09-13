#ifndef levels_h
#define levels_h

extern char* outro;
extern char* grats;
extern char* intro;
extern char* stage1;
extern char* stage2;
extern char* stage3;
extern char* stage4;
extern char* stage5;

#endif /* levels_h */

#if __INCLUDE_LEVEL__ == 0

char* intro =
    "frame 0 cube 0 label CONTROL_WITH red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 label LEFT/RIGHT/RETURN\n"
    "frame 200 label _\n"
    "frame 250 label GO\n"
    "frame 300 label WITH_THE_FLOW red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 350 next true\n";

char* grats =
    "frame 0 cube 0 label CONGRATULATIONS red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 240 label GO_FOR_THE_NEXT_LEVEL red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 0\n";

char* outro =
    "frame 0 cube 0 label AMAZING red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 240 label YOU_WENT red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 480 label WITH_THE_FLOW red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 700 red 0.0 green 0.0 blue 0.0 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 0\n";

char* stage1 =
    "frame 0 cube 0 label CEREBRAL_CORTEX red 0.3 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 cube 1 widthA 21 modeA random\n"
    "frame 1320 diffA 1.5 widthA 20\n"
    "frame 2520 diffA 2.0 widthA 20\n"
    "frame 3600 modeA linear widthA 40\n"
    "frame 3840 next true\n";

char* stage2 =
    "frame 0 cube 0 label PARIETAL_LOBE red 0.8 green 0.3 blue .3 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 cube 1 widthA 20 widthB 15 modeA random modeB random\n"
    "frame 1320 widthA 18 widthB 13 diffA 1.5 diffB 1.5\n"
    "frame 2520 widthA 18 widthB 13 diffA 2.0 diffB 2.0\n"
    "frame 3600 widthA 40 widthB 40 modeA linear modeB linear\n"
    "frame 3840 next true\n";

char* stage3 =
    "frame 0 cube 0 label CENTRAL_SULCUS red 0.3 green 0.8 blue .3 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 cube 1 widthA 20 widthB 15 diffA 2.0 diffB 2.0 modeA zigzag modeB strong\n"
    "frame 1320 widthA 19 widthB 14 diffA 2.0 diffB 2.0 modeA wave modeB zigzag\n"
    "frame 2520 widthA 18 widthB 13 diffA 2.5 diffB 2.5 modeA strong modeB random\n"
    "frame 3600 modeA linear modeB linear widthA 40 widthB 40\n"
    "frame 3840 next true\n";

char* stage4 =
    "frame 0 cube 0 label FRONTAL_LOBE red 0.3 green 0.8 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 cube 1 widthA 16 widthB 15 widthC 10 diffA 2.0 diffB 2.0 diffC 2.0 modeA random modeB random modeC random\n"
    "frame 1320 widthA 15 widthB 15 widthC 10 diffA 3.0 diffB 2.0 diffC 2.0 modeA strong modeB zigzag modeC wave\n"
    "frame 2520 widthA 15 widthB 12 widthC 12 diffA 3.0 diffB 3.0 diffC 3.0 modeA random modeB zigzag modeC wave\n"
    "frame 3600 modeA linear modeB linear modeC linear widthA 40 widthB 40 widthc 40\n"
    "frame 3840 next true\n";

char* stage5 =
    "frame 0 cube 0 label AUDITORY_CORTEX red 0.8 green 0.3 blue .8 diffA 1.0 diffB 1.0 diffC 1.0 diffD 1.0 widthA 40 widthB 40 widthC 40 widthD 40 modeA linear modeB still modeC still modeD still\n"
    "frame 120 cube 1 widthA 12 widthB 12 widthC 2 widthD 12 diffA 2.0 diffB 2.0 diffC 2.0 diffD 2.0 modeA random modeB strong modeC wave modeD zigzag\n"
    "frame 1320 widthA 10 widthB 12 widthC 10 widthD 12 diffA 3.0 diffB 2.0 diffC 2.0 diffD 2.0 modeA random modeB strong modeC wave modeD zigzag\n"
    "frame 2520 widthA 10 widthB 10 widthC 10 widthD 10 diffA 3.0 diffB 3.0 diffC 3.0 diffD 3.0 modeA random modeB strong modeC wave modeD zigzag\n"
    "frame 3600 modeA linear modeB linear modeC linear modeD linear widthA 40 widthB 40 widthC 40 widthD 40\n"
    "frame 3840 next true\n";

#endif

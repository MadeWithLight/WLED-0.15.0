/*
  WS2812FX.h - Library for WS2812 LED effects.
  Harm Aldick - 2016
  www.aldick.org

  Copyright (c) 2016  Harm Aldick
  Licensed under the EUPL v. 1.2 or later
  Adapted from code originally licensed under the MIT license

  Modified for WLED
*/

#ifndef WS2812FX_h
#define WS2812FX_h

#include <vector>

#include "const.h"

#define FASTLED_INTERNAL //remove annoying pragma messages
#define USE_GET_MILLISECOND_TIMER
#include "FastLED.h"

#define DEFAULT_BRIGHTNESS (uint8_t)127
#define DEFAULT_MODE       (uint8_t)0
#define DEFAULT_SPEED      (uint8_t)128
#define DEFAULT_INTENSITY  (uint8_t)128
#define DEFAULT_COLOR      (uint32_t)0xFFAA00
#define DEFAULT_C1         (uint8_t)128
#define DEFAULT_C2         (uint8_t)128
#define DEFAULT_C3         (uint8_t)16

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

//color mangling macros
#ifndef RGBW32
#define RGBW32(r,g,b,w) (uint32_t((byte(w) << 24) | (byte(r) << 16) | (byte(g) << 8) | (byte(b))))
#endif

/* Not used in all effects yet */
#define WLED_FPS         42
#define FRAMETIME_FIXED  (1000/WLED_FPS)
#define FRAMETIME        strip.getFrameTime()

// FPS calculation (can be defined as compile flag for debugging)
#ifndef FPS_CALC_AVG
#define FPS_CALC_AVG 7 // average FPS calculation over this many frames (moving average)
#endif
#ifndef FPS_MULTIPLIER
#define FPS_MULTIPLIER 1 // dev option: multiplier to get sub-frame FPS without floats
#endif
#define FPS_CALC_SHIFT 7 // bit shift for fixed point math

/* each segment uses 82 bytes of SRAM memory, so if you're application fails because of
  insufficient memory, decreasing MAX_NUM_SEGMENTS may help */
#ifdef ESP8266
  #define MAX_NUM_SEGMENTS    16
  /* How much data bytes all segments combined may allocate */
  #define MAX_SEGMENT_DATA  5120
#else
  #ifndef MAX_NUM_SEGMENTS
    #define MAX_NUM_SEGMENTS  32
  #endif
  #if defined(ARDUINO_ARCH_ESP32S2)
    #define MAX_SEGMENT_DATA  MAX_NUM_SEGMENTS*768 // 24k by default (S2 is short on free RAM)
  #else
    #define MAX_SEGMENT_DATA  MAX_NUM_SEGMENTS*1280 // 40k by default
  #endif
#endif

/* How much data bytes each segment should max allocate to leave enough space for other segments,
  assuming each segment uses the same amount of data. 256 for ESP8266, 640 for ESP32. */
#define FAIR_DATA_PER_SEG (MAX_SEGMENT_DATA / strip.getMaxSegments())

#define MIN_SHOW_DELAY   (_frametime < 16 ? 8 : 15)

#define NUM_COLORS       3 /* number of colors per segment */
#define SEGMENT          strip._segments[strip.getCurrSegmentId()]
#define SEGENV           strip._segments[strip.getCurrSegmentId()]
//#define SEGCOLOR(x)      strip._segments[strip.getCurrSegmentId()].currentColor(x, strip._segments[strip.getCurrSegmentId()].colors[x])
//#define SEGLEN           strip._segments[strip.getCurrSegmentId()].virtualLength()
#define SEGCOLOR(x)      strip.segColor(x) /* saves us a few kbytes of code */
#define SEGPALETTE       Segment::getCurrentPalette()
#define SEGLEN           strip._virtualSegmentLength /* saves us a few kbytes of code */
#define SPEED_FORMULA_L  (5U + (50U*(255U - SEGMENT.speed))/SEGLEN)

// some common colors
#define RED        (uint32_t)0xFF0000
#define GREEN      (uint32_t)0x00FF00
#define BLUE       (uint32_t)0x0000FF
#define WHITE      (uint32_t)0xFFFFFF
#define BLACK      (uint32_t)0x000000
#define YELLOW     (uint32_t)0xFFFF00
#define CYAN       (uint32_t)0x00FFFF
#define MAGENTA    (uint32_t)0xFF00FF
#define PURPLE     (uint32_t)0x400080
#define ORANGE     (uint32_t)0xFF3000
#define PINK       (uint32_t)0xFF1493
#define GREY       (uint32_t)0x808080
#define GRAY       GREY
#define DARKGREY   (uint32_t)0x333333
#define DARKGRAY   DARKGREY
#define ULTRAWHITE (uint32_t)0xFFFFFFFF
#define DARKSLATEGRAY (uint32_t)0x2F4F4F
#define DARKSLATEGREY DARKSLATEGRAY

// segment options
#define NO_OPTIONS   (uint16_t)0x0000
#define TRANSPOSED   (uint16_t)0x0100 // rotated 90deg & reversed
#define MIRROR_Y_2D  (uint16_t)0x0080
#define REVERSE_Y_2D (uint16_t)0x0040
#define RESET_REQ    (uint16_t)0x0020
#define FROZEN       (uint16_t)0x0010
#define MIRROR       (uint16_t)0x0008
#define SEGMENT_ON   (uint16_t)0x0004
#define REVERSE      (uint16_t)0x0002
#define SELECTED     (uint16_t)0x0001

#define FX_MODE_STATIC                   0
#define FX_MODE_BLINK                    1
#define FX_MODE_BREATH                   2
#define FX_MODE_COLOR_WIPE               3
#define FX_MODE_COLOR_WIPE_RANDOM        4
#define FX_MODE_RANDOM_COLOR             5
#define FX_MODE_COLOR_SWEEP              6
#define FX_MODE_DYNAMIC                  7
#define FX_MODE_RAINBOW                  8
#define FX_MODE_RAINBOW_CYCLE            9
#define FX_MODE_SCAN                    10
#define FX_MODE_DUAL_SCAN               11
#define FX_MODE_FADE                    12
#define FX_MODE_THEATER_CHASE           13
#define FX_MODE_THEATER_CHASE_RAINBOW   14
#define FX_MODE_RUNNING_LIGHTS          15
#define FX_MODE_SAW                     16
#define FX_MODE_TWINKLE                 17
#define FX_MODE_DISSOLVE                18
#define FX_MODE_DISSOLVE_RANDOM         19  // candidate for removal (use Dissolve with with check 3)
#define FX_MODE_SPARKLE                 20
#define FX_MODE_FLASH_SPARKLE           21
#define FX_MODE_HYPER_SPARKLE           22
#define FX_MODE_STROBE                  23
#define FX_MODE_STROBE_RAINBOW          24
#define FX_MODE_MULTI_STROBE            25
#define FX_MODE_BLINK_RAINBOW           26
#define FX_MODE_ANDROID                 27
#define FX_MODE_CHASE_COLOR             28
#define FX_MODE_CHASE_RANDOM            29
#define FX_MODE_CHASE_RAINBOW           30
#define FX_MODE_CHASE_FLASH             31
#define FX_MODE_CHASE_FLASH_RANDOM      32
#define FX_MODE_CHASE_RAINBOW_WHITE     33
#define FX_MODE_COLORFUL                34
#define FX_MODE_TRAFFIC_LIGHT           35
#define FX_MODE_COLOR_SWEEP_RANDOM      36
#define FX_MODE_RUNNING_COLOR           37
#define FX_MODE_AURORA                  38
#define FX_MODE_RUNNING_RANDOM          39
#define FX_MODE_LARSON_SCANNER          40
#define FX_MODE_COMET                   41
#define FX_MODE_FIREWORKS               42
#define FX_MODE_RAIN                    43
#define FX_MODE_TETRIX                  44  //was Merry Christmas prior to 0.12.0 (use "Chase 2" with Red/Green)
#define FX_MODE_FIRE_FLICKER            45
#define FX_MODE_GRADIENT                46
#define FX_MODE_LOADING                 47
#define FX_MODE_ROLLINGBALLS            48  //was Police before 0.14
#define FX_MODE_FAIRY                   49  //was Police All prior to 0.13.0-b6 (use "Two Dots" with Red/Blue and full intensity)
#define FX_MODE_TWO_DOTS                50
#define FX_MODE_FAIRYTWINKLE            51  //was Two Areas prior to 0.13.0-b6 (use "Two Dots" with full intensity)
#define FX_MODE_RUNNING_DUAL            52
// #define FX_MODE_HALLOWEEN               53  // removed in 0.14!
#define FX_MODE_TRICOLOR_CHASE          54
#define FX_MODE_TRICOLOR_WIPE           55
#define FX_MODE_TRICOLOR_FADE           56
#define FX_MODE_LIGHTNING               57
#define FX_MODE_ICU                     58
#define FX_MODE_MULTI_COMET             59
#define FX_MODE_DUAL_LARSON_SCANNER     60  // candidate for removal (use Scanner with with check 1)
#define FX_MODE_RANDOM_CHASE            61
#define FX_MODE_OSCILLATE               62
#define FX_MODE_PRIDE_2015              63
#define FX_MODE_JUGGLE                  64
#define FX_MODE_PALETTE                 65
#define FX_MODE_FIRE_2012               66
#define FX_MODE_COLORWAVES              67
#define FX_MODE_BPM                     68
#define FX_MODE_FILLNOISE8              69
#define FX_MODE_NOISE16_1               70
#define FX_MODE_NOISE16_2               71
#define FX_MODE_NOISE16_3               72
#define FX_MODE_NOISE16_4               73
#define FX_MODE_COLORTWINKLE            74
#define FX_MODE_LAKE                    75
#define FX_MODE_METEOR                  76
#define FX_MODE_METEOR_SMOOTH           77
#define FX_MODE_RAILWAY                 78
#define FX_MODE_RIPPLE                  79
#define FX_MODE_TWINKLEFOX              80
#define FX_MODE_TWINKLECAT              81
#define FX_MODE_HALLOWEEN_EYES          82
#define FX_MODE_STATIC_PATTERN          83
#define FX_MODE_TRI_STATIC_PATTERN      84
#define FX_MODE_SPOTS                   85
#define FX_MODE_SPOTS_FADE              86
#define FX_MODE_GLITTER                 87
#define FX_MODE_CANDLE                  88
#define FX_MODE_STARBURST               89
#define FX_MODE_EXPLODING_FIREWORKS     90
#define FX_MODE_BOUNCINGBALLS           91
#define FX_MODE_SINELON                 92
#define FX_MODE_SINELON_DUAL            93
#define FX_MODE_SINELON_RAINBOW         94
#define FX_MODE_POPCORN                 95
#define FX_MODE_DRIP                    96
#define FX_MODE_PLASMA                  97
#define FX_MODE_PERCENT                 98
#define FX_MODE_RIPPLE_RAINBOW          99
#define FX_MODE_HEARTBEAT              100
#define FX_MODE_PACIFICA               101
#define FX_MODE_CANDLE_MULTI           102
#define FX_MODE_SOLID_GLITTER          103  // candidate for removal (use glitter)
#define FX_MODE_SUNRISE                104
#define FX_MODE_PHASED                 105
#define FX_MODE_TWINKLEUP              106
#define FX_MODE_NOISEPAL               107
#define FX_MODE_SINEWAVE               108
#define FX_MODE_PHASEDNOISE            109
#define FX_MODE_FLOW                   110
#define FX_MODE_CHUNCHUN               111
#define FX_MODE_DANCING_SHADOWS        112
#define FX_MODE_WASHING_MACHINE        113
#define FX_MODE_2DPLASMAROTOZOOM       114 // was Candy Cane prior to 0.14 (use Chase 2)
#define FX_MODE_BLENDS                 115
#define FX_MODE_TV_SIMULATOR           116
#define FX_MODE_DYNAMIC_SMOOTH         117 // candidate for removal (check3 in dynamic)

// new 0.14 2D effects
#define FX_MODE_2DSPACESHIPS           118 //gap fill
#define FX_MODE_2DCRAZYBEES            119 //gap fill
#define FX_MODE_2DGHOSTRIDER           120 //gap fill
#define FX_MODE_2DBLOBS                121 //gap fill
#define FX_MODE_2DSCROLLTEXT           122 //gap fill
#define FX_MODE_2DDRIFTROSE            123 //gap fill
#define FX_MODE_2DDISTORTIONWAVES      124 //gap fill
#define FX_MODE_2DSOAP                 125 //gap fill
#define FX_MODE_2DOCTOPUS              126 //gap fill
#define FX_MODE_2DWAVINGCELL           127 //gap fill

// WLED-SR effects (SR compatible IDs !!!)
#define FX_MODE_PIXELS                 128
#define FX_MODE_PIXELWAVE              129
#define FX_MODE_JUGGLES                130
#define FX_MODE_MATRIPIX               131
#define FX_MODE_GRAVIMETER             132
#define FX_MODE_PLASMOID               133
#define FX_MODE_PUDDLES                134
#define FX_MODE_MIDNOISE               135
#define FX_MODE_NOISEMETER             136
#define FX_MODE_FREQWAVE               137
#define FX_MODE_FREQMATRIX             138
#define FX_MODE_2DGEQ                  139
#define FX_MODE_WATERFALL              140
#define FX_MODE_FREQPIXELS             141
#define FX_MODE_BINMAP                 142
#define FX_MODE_NOISEFIRE              143
#define FX_MODE_PUDDLEPEAK             144
#define FX_MODE_NOISEMOVE              145
#define FX_MODE_2DNOISE                146
#define FX_MODE_PERLINMOVE             147
#define FX_MODE_RIPPLEPEAK             148
#define FX_MODE_2DFIRENOISE            149
#define FX_MODE_2DSQUAREDSWIRL         150
// #define FX_MODE_2DFIRE2012             151
#define FX_MODE_2DDNA                  152
#define FX_MODE_2DMATRIX               153
#define FX_MODE_2DMETABALLS            154
#define FX_MODE_FREQMAP                155
#define FX_MODE_GRAVCENTER             156
#define FX_MODE_GRAVCENTRIC            157
#define FX_MODE_GRAVFREQ               158
#define FX_MODE_DJLIGHT                159
#define FX_MODE_2DFUNKYPLANK           160
//#define FX_MODE_2DCENTERBARS           161
#define FX_MODE_2DPULSER               162
#define FX_MODE_BLURZ                  163
#define FX_MODE_2DDRIFT                164
#define FX_MODE_2DWAVERLY              165
#define FX_MODE_2DSUNRADIATION         166
#define FX_MODE_2DCOLOREDBURSTS        167
#define FX_MODE_2DJULIA                168
// #define FX_MODE_2DPOOLNOISE            169 //have been removed in WLED SR in the past because of low mem but should be added back
// #define FX_MODE_2DTWISTER              170 //have been removed in WLED SR in the past because of low mem but should be added back
// #define FX_MODE_2DCAELEMENTATY         171 //have been removed in WLED SR in the past because of low mem but should be added back
#define FX_MODE_2DGAMEOFLIFE           172
#define FX_MODE_2DTARTAN               173
#define FX_MODE_2DPOLARLIGHTS          174
#define FX_MODE_2DSWIRL                175
#define FX_MODE_2DLISSAJOUS            176
#define FX_MODE_2DFRIZZLES             177
#define FX_MODE_2DPLASMABALL           178
#define FX_MODE_FLOWSTRIPE             179
#define FX_MODE_2DHIPHOTIC             180
#define FX_MODE_2DSINDOTS              181
#define FX_MODE_2DDNASPIRAL            182
#define FX_MODE_2DBLACKHOLE            183
#define FX_MODE_WAVESINS               184
#define FX_MODE_ROCKTAVES              185
#define FX_MODE_2DAKEMI                186
#define FX_MODE_FIRE_2025_SR           187
#define FX_MODE_FIRE_2025              188
#define MODE_COUNT                     189

typedef enum mapping1D2D {
  M12_Pixels = 0,
  M12_pBar = 1,
  M12_pArc = 2,
  M12_pCorner = 3,
  M12_sPinwheel = 4
} mapping1D2D_t;

// segment, 80 bytes
typedef struct Segment {
  public:
    uint16_t start; // start index / start X coordinate 2D (left)
    uint16_t stop;  // stop index / stop X coordinate 2D (right); segment is invalid if stop == 0
    uint16_t offset;
    uint8_t  speed;
    uint8_t  intensity;
    uint8_t  palette;
    uint8_t  mode;
    union {
      uint16_t options; //bit pattern: msb first: [transposed mirrorY reverseY] transitional (tbd) paused needspixelstate mirrored on reverse selected
      struct {
        bool    selected    : 1;  //     0 : selected
        bool    reverse     : 1;  //     1 : reversed
        bool    on          : 1;  //     2 : is On
        bool    mirror      : 1;  //     3 : mirrored
        bool    freeze      : 1;  //     4 : paused/frozen
        bool    reset       : 1;  //     5 : indicates that Segment runtime requires reset
        bool    reverse_y   : 1;  //     6 : reversed Y (2D)
        bool    mirror_y    : 1;  //     7 : mirrored Y (2D)
        bool    transpose   : 1;  //     8 : transposed (2D, swapped X & Y)
        uint8_t map1D2D     : 3;  //  9-11 : mapping for 1D effect on 2D (0-use as strip, 1-expand vertically, 2-circular/arc, 3-rectangular/corner, ...)
        uint8_t soundSim    : 2;  // 12-13 : 0-3 sound simulation types ("soft" & "hard" or "on"/"off")
        uint8_t set         : 2;  // 14-15 : 0-3 UI segment sets/groups
      };
    };
    uint8_t  grouping, spacing;
    uint8_t  opacity;
    uint32_t colors[NUM_COLORS];
    uint8_t  cct;                 //0==1900K, 255==10091K
    uint8_t  custom1, custom2;    // custom FX parameters/sliders
    struct {
      uint8_t custom3 : 5;        // reduced range slider (0-31)
      bool    check1  : 1;        // checkmark 1
      bool    check2  : 1;        // checkmark 2
      bool    check3  : 1;        // checkmark 3
    };
    uint8_t startY;  // start Y coodrinate 2D (top); there should be no more than 255 rows
    uint8_t stopY;   // stop Y coordinate 2D (bottom); there should be no more than 255 rows
    char    *name;

    // runtime data
    unsigned long next_time;  // millis() of next update
    uint32_t step;  // custom "step" var
    uint32_t call;  // call counter
    uint16_t aux0;  // custom var
    uint16_t aux1;  // custom var
    byte     *data; // effect data pointer
    static uint16_t maxWidth, maxHeight;  // these define matrix width & height (max. segment dimensions)

    typedef struct TemporarySegmentData {
      uint16_t _optionsT;
      uint32_t _colorT[NUM_COLORS];
      uint8_t  _speedT;
      uint8_t  _intensityT;
      uint8_t  _custom1T, _custom2T;   // custom FX parameters/sliders
      struct {
        uint8_t _custom3T : 5;        // reduced range slider (0-31)
        bool    _check1T  : 1;        // checkmark 1
        bool    _check2T  : 1;        // checkmark 2
        bool    _check3T  : 1;        // checkmark 3
      };
      uint16_t _aux0T;
      uint16_t _aux1T;
      uint32_t _stepT;
      uint32_t _callT;
      uint8_t *_dataT;
      uint16_t _dataLenT;
      TemporarySegmentData()
        : _dataT(nullptr) // just in case...
        , _dataLenT(0)
      {}
    } tmpsegd_t;

  private:
    union {
      uint8_t  _capabilities;
      struct {
        bool    _isRGB    : 1;
        bool    _hasW     : 1;
        bool    _isCCT    : 1;
        bool    _manualW  : 1;
        uint8_t _reserved : 4;
      };
    };
    uint16_t        _dataLen;
    static uint16_t _usedSegmentData;

    // perhaps this should be per segment, not static
    static CRGBPalette16 _currentPalette;     // palette used for current effect (includes transition, used in color_from_palette())
    static CRGBPalette16 _randomPalette;      // actual random palette
    static CRGBPalette16 _newRandomPalette;   // target random palette
    static uint16_t _lastPaletteChange;       // last random palette change time in millis()/1000
    static uint16_t _lastPaletteBlend;        // blend palette according to set Transition Delay in millis()%0xFFFF
    #ifndef WLED_DISABLE_MODE_BLEND
    static bool          _modeBlend;          // mode/effect blending semaphore
    #endif

    // transition data, valid only if transitional==true, holds values during transition (72 bytes)
    struct Transition {
      #ifndef WLED_DISABLE_MODE_BLEND
      tmpsegd_t     _segT;        // previous segment environment
      uint8_t       _modeT;       // previous mode/effect
      #else
      uint32_t      _colorT[NUM_COLORS];
      #endif
      uint8_t       _briT;        // temporary brightness
      uint8_t       _cctT;        // temporary CCT
      CRGBPalette16 _palT;        // temporary palette
      uint8_t       _prevPaletteBlends; // number of previous palette blends (there are max 255 blends possible)
      unsigned long _start;       // must accommodate millis()
      uint16_t      _dur;
      Transition(uint16_t dur=750)
        : _palT(CRGBPalette16(CRGB::Black))
        , _prevPaletteBlends(0)
        , _start(millis())
        , _dur(dur)
      {}
    } *_t;

  public:

    Segment(uint16_t sStart=0, uint16_t sStop=30) :
      start(sStart),
      stop(sStop),
      offset(0),
      speed(DEFAULT_SPEED),
      intensity(DEFAULT_INTENSITY),
      palette(0),
      mode(DEFAULT_MODE),
      options(SELECTED | SEGMENT_ON),
      grouping(1),
      spacing(0),
      opacity(255),
      colors{DEFAULT_COLOR,BLACK,BLACK},
      cct(127),
      custom1(DEFAULT_C1),
      custom2(DEFAULT_C2),
      custom3(DEFAULT_C3),
      check1(false),
      check2(false),
      check3(false),
      startY(0),
      stopY(1),
      name(nullptr),
      next_time(0),
      step(0),
      call(0),
      aux0(0),
      aux1(0),
      data(nullptr),
      _capabilities(0),
      _dataLen(0),
      _t(nullptr)
    {
      #ifdef WLED_DEBUG
      //Serial.printf("-- Creating segment: %p\n", this);
      #endif
    }

    Segment(uint16_t sStartX, uint16_t sStopX, uint16_t sStartY, uint16_t sStopY) : Segment(sStartX, sStopX) {
      startY = sStartY;
      stopY  = sStopY;
    }

    Segment(const Segment &orig); // copy constructor
    Segment(Segment &&orig) noexcept; // move constructor

    ~Segment() {
      #ifdef WLED_DEBUG
      //Serial.printf("-- Destroying segment: %p", this);
      //if (name) Serial.printf(" %s (%p)", name, name);
      //if (data) Serial.printf(" %d->(%p)", (int)_dataLen, data);
      //Serial.println();
      #endif
      if (name) { delete[] name; name = nullptr; }
      stopTransition();
      deallocateData();
    }

    Segment& operator= (const Segment &orig); // copy assignment
    Segment& operator= (Segment &&orig) noexcept; // move assignment

#ifdef WLED_DEBUG
    size_t getSize() const { return sizeof(Segment) + (data?_dataLen:0) + (name?strlen(name):0) + (_t?sizeof(Transition):0); }
#endif

    inline bool     getOption(uint8_t n) const { return ((options >> n) & 0x01); }
    inline bool     isSelected()         const { return selected; }
    inline bool     isInTransition()     const { return _t != nullptr; }
    inline bool     isActive()           const { return stop > start; }
    inline bool     is2D()               const { return (width()>1 && height()>1); }
    inline bool     hasRGB()             const { return _isRGB; }
    inline bool     hasWhite()           const { return _hasW; }
    inline bool     isCCT()              const { return _isCCT; }
    inline uint16_t width()              const { return isActive() ? (stop - start) : 0; }  // segment width in physical pixels (length if 1D)
    inline uint16_t height()             const { return stopY - startY; }                   // segment height (if 2D) in physical pixels (it *is* always >=1)
    inline uint16_t length()             const { return width() * height(); }               // segment length (count) in physical pixels
    inline uint16_t groupLength()        const { return grouping + spacing; }
    inline uint8_t  getLightCapabilities() const { return _capabilities; }

    inline static uint16_t getUsedSegmentData()    { return _usedSegmentData; }
    inline static void addUsedSegmentData(int len) { _usedSegmentData += len; }
    #ifndef WLED_DISABLE_MODE_BLEND
    inline static void modeBlend(bool blend)       { _modeBlend = blend; }
    #endif
    static void     handleRandomPalette();
    inline static const CRGBPalette16 &getCurrentPalette() { return Segment::_currentPalette; }

    void    setUp(uint16_t i1, uint16_t i2, uint8_t grp=1, uint8_t spc=0, uint16_t ofs=UINT16_MAX, uint16_t i1Y=0, uint16_t i2Y=1);
    Segment &setColor(uint8_t slot, uint32_t c);
    Segment &setCCT(uint16_t k);
    Segment &setOpacity(uint8_t o);
    Segment &setOption(uint8_t n, bool val);
    Segment &setMode(uint8_t fx, bool loadDefaults = false);
    Segment &setPalette(uint8_t pal);
    uint8_t differs(Segment& b) const;
    void    refreshLightCapabilities();

    // runtime data functions
    inline uint16_t dataSize() const { return _dataLen; }
    bool allocateData(size_t len);  // allocates effect data buffer in heap and clears it
    void deallocateData();          // deallocates (frees) effect data buffer from heap
    void resetIfRequired();         // sets all SEGENV variables to 0 and clears data buffer
    /**
      * Flags that before the next effect is calculated,
      * the internal segment state should be reset.
      * Call resetIfRequired before calling the next effect function.
      * Safe to call from interrupts and network requests.
      */
    inline Segment &markForReset() { reset = true; return *this; }  // setOption(SEG_OPTION_RESET, true)

    // transition functions
    void     startTransition(uint16_t dur);     // transition has to start before actual segment values change
    void     stopTransition();                  // ends transition mode by destroying transition structure (does nothing if not in transition)
    inline void handleTransition() { if (progress() == 0xFFFFU) stopTransition(); }
    #ifndef WLED_DISABLE_MODE_BLEND
    void     swapSegenv(tmpsegd_t &tmpSegD);    // copies segment data into specifed buffer, if buffer is not a transition buffer, segment data is overwritten from transition buffer
    void     restoreSegenv(tmpsegd_t &tmpSegD); // restores segment data from buffer, if buffer is not transition buffer, changed values are copied to transition buffer
    #endif
    [[gnu::hot]] uint16_t progress() const;                  // transition progression between 0-65535
    [[gnu::hot]] uint8_t  currentBri(bool useCct = false) const; // current segment brightness/CCT (blended while in transition)
    uint8_t  currentMode() const;                            // currently active effect/mode (while in transition)
    [[gnu::hot]] uint32_t currentColor(uint8_t slot) const;  // currently active segment color (blended while in transition)
    CRGBPalette16 &loadPalette(CRGBPalette16 &tgt, uint8_t pal);
    void     setCurrentPalette();

    // 1D strip
    [[gnu::hot]] uint16_t virtualLength() const;
    [[gnu::hot]] void setPixelColor(int n, uint32_t c); // set relative pixel within segment with color
    inline void setPixelColor(unsigned n, uint32_t c)                    { setPixelColor(int(n), c); }
    inline void setPixelColor(int n, byte r, byte g, byte b, byte w = 0) { setPixelColor(n, RGBW32(r,g,b,w)); }
    inline void setPixelColor(int n, CRGB c)                             { setPixelColor(n, RGBW32(c.r,c.g,c.b,0)); }
    #ifdef WLED_USE_AA_PIXELS
    void setPixelColor(float i, uint32_t c, bool aa = true);
    inline void setPixelColor(float i, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0, bool aa = true) { setPixelColor(i, RGBW32(r,g,b,w), aa); }
    inline void setPixelColor(float i, CRGB c, bool aa = true)                                         { setPixelColor(i, RGBW32(c.r,c.g,c.b,0), aa); }
    #endif
    [[gnu::hot]] uint32_t getPixelColor(int i) const;
    // 1D support functions (some implement 2D as well)
    void blur(uint8_t, bool smear = false);
    void fill(uint32_t c);
    void fade_out(uint8_t r);
    void fadeToBlackBy(uint8_t fadeBy);
    inline void blendPixelColor(int n, uint32_t color, uint8_t blend)    { setPixelColor(n, color_blend(getPixelColor(n), color, blend)); }
    inline void blendPixelColor(int n, CRGB c, uint8_t blend)            { blendPixelColor(n, RGBW32(c.r,c.g,c.b,0), blend); }
    inline void addPixelColor(int n, uint32_t color, bool fast = false)  { setPixelColor(n, color_add(getPixelColor(n), color, fast)); }
    inline void addPixelColor(int n, byte r, byte g, byte b, byte w = 0, bool fast = false) { addPixelColor(n, RGBW32(r,g,b,w), fast); }
    inline void addPixelColor(int n, CRGB c, bool fast = false)          { addPixelColor(n, RGBW32(c.r,c.g,c.b,0), fast); }
    inline void fadePixelColor(uint16_t n, uint8_t fade)                 { setPixelColor(n, color_fade(getPixelColor(n), fade, true)); }
    [[gnu::hot]] uint32_t color_from_palette(uint16_t, bool mapping, bool wrap, uint8_t mcol, uint8_t pbri = 255) const;
    [[gnu::hot]] uint32_t color_wheel(uint8_t pos) const;

    // 2D Blur: shortcuts for bluring columns or rows only (50% faster than full 2D blur)
    inline void blurCols(fract8 blur_amount, bool smear = false) { // blur all columns
      const unsigned cols = virtualWidth();
      for (unsigned k = 0; k < cols; k++) blurCol(k, blur_amount, smear); 
    }
    inline void blurRows(fract8 blur_amount, bool smear = false) { // blur all rows
      const unsigned rows = virtualHeight();
      for ( unsigned i = 0; i < rows; i++) blurRow(i, blur_amount, smear); 
    }

    // 2D matrix
    [[gnu::hot]] unsigned virtualWidth()  const; // segment width in virtual pixels (accounts for groupping and spacing)
    [[gnu::hot]] unsigned virtualHeight() const; // segment height in virtual pixels (accounts for groupping and spacing)
    inline unsigned nrOfVStrips() const {        // returns number of virtual vertical strips in 2D matrix (used to expand 1D effects into 2D)
    #ifndef WLED_DISABLE_2D
      return (is2D() &&  map1D2D == M12_pBar) ? virtualWidth() : 1;
    #else
      return 1;
    #endif
    }
  #ifndef WLED_DISABLE_2D
    [[gnu::hot]] uint16_t XY(int x, int y);      // support function to get relative index within segment
    [[gnu::hot]] void setPixelColorXY(int x, int y, uint32_t c); // set relative pixel within segment with color
    inline void setPixelColorXY(unsigned x, unsigned y, uint32_t c)               { setPixelColorXY(int(x), int(y), c); }
    inline void setPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0) { setPixelColorXY(x, y, RGBW32(r,g,b,w)); }
    inline void setPixelColorXY(int x, int y, CRGB c)                             { setPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0)); }
    inline void setPixelColorXY(unsigned x, unsigned y, CRGB c)                   { setPixelColorXY(int(x), int(y), RGBW32(c.r,c.g,c.b,0)); }
    #ifdef WLED_USE_AA_PIXELS
    void setPixelColorXY(float x, float y, uint32_t c, bool aa = true);
    inline void setPixelColorXY(float x, float y, byte r, byte g, byte b, byte w = 0, bool aa = true) { setPixelColorXY(x, y, RGBW32(r,g,b,w), aa); }
    inline void setPixelColorXY(float x, float y, CRGB c, bool aa = true)                             { setPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0), aa); }
    #endif
    [[gnu::hot]] uint32_t getPixelColorXY(int x, int y) const;
    // 2D support functions
    inline void blendPixelColorXY(uint16_t x, uint16_t y, uint32_t color, uint8_t blend) { setPixelColorXY(x, y, color_blend(getPixelColorXY(x,y), color, blend)); }
    inline void blendPixelColorXY(uint16_t x, uint16_t y, CRGB c, uint8_t blend)         { blendPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0), blend); }
    inline void addPixelColorXY(int x, int y, uint32_t color, bool fast = false)         { setPixelColorXY(x, y, color_add(getPixelColorXY(x,y), color, fast)); }
    inline void addPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0, bool fast = false) { addPixelColorXY(x, y, RGBW32(r,g,b,w), fast); }
    inline void addPixelColorXY(int x, int y, CRGB c, bool fast = false)                             { addPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0), fast); }
    inline void fadePixelColorXY(uint16_t x, uint16_t y, uint8_t fade)                               { setPixelColorXY(x, y, color_fade(getPixelColorXY(x,y), fade, true)); }
    void box_blur(unsigned r = 1U, bool smear = false); // 2D box blur
    void blur2D(uint8_t blur_amount, bool smear = false);
    void blurRow(uint32_t row, fract8 blur_amount, bool smear = false);
    void blurCol(uint32_t col, fract8 blur_amount, bool smear = false);
    void moveX(int8_t delta, bool wrap = false);
    void moveY(int8_t delta, bool wrap = false);
    void move(uint8_t dir, uint8_t delta, bool wrap = false);
    void drawCircle(uint16_t cx, uint16_t cy, uint8_t radius, uint32_t c, bool soft = false);
    inline void drawCircle(uint16_t cx, uint16_t cy, uint8_t radius, CRGB c, bool soft = false) { drawCircle(cx, cy, radius, RGBW32(c.r,c.g,c.b,0), soft); }
    void fillCircle(uint16_t cx, uint16_t cy, uint8_t radius, uint32_t c, bool soft = false);
    inline void fillCircle(uint16_t cx, uint16_t cy, uint8_t radius, CRGB c, bool soft = false) { fillCircle(cx, cy, radius, RGBW32(c.r,c.g,c.b,0), soft); }
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t c, bool soft = false);
    inline void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, CRGB c, bool soft = false) { drawLine(x0, y0, x1, y1, RGBW32(c.r,c.g,c.b,0), soft); } // automatic inline
    void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, uint32_t color, uint32_t col2 = 0, int8_t rotate = 0);
    inline void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, CRGB c) { drawCharacter(chr, x, y, w, h, RGBW32(c.r,c.g,c.b,0)); } // automatic inline
    inline void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, CRGB c, CRGB c2, int8_t rotate = 0) { drawCharacter(chr, x, y, w, h, RGBW32(c.r,c.g,c.b,0), RGBW32(c2.r,c2.g,c2.b,0), rotate); } // automatic inline
    void wu_pixel(uint32_t x, uint32_t y, CRGB c);
    inline void blur2d(fract8 blur_amount) { blur(blur_amount); }
    inline void fill_solid(CRGB c) { fill(RGBW32(c.r,c.g,c.b,0)); }
  #else
    inline uint16_t XY(uint16_t x, uint16_t y)                                    { return x; }
    inline void setPixelColorXY(int x, int y, uint32_t c)                         { setPixelColor(x, c); }
    inline void setPixelColorXY(unsigned x, unsigned y, uint32_t c)               { setPixelColor(int(x), c); }
    inline void setPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0) { setPixelColor(x, RGBW32(r,g,b,w)); }
    inline void setPixelColorXY(int x, int y, CRGB c)                             { setPixelColor(x, RGBW32(c.r,c.g,c.b,0)); }
    inline void setPixelColorXY(unsigned x, unsigned y, CRGB c)                   { setPixelColor(int(x), RGBW32(c.r,c.g,c.b,0)); }
    #ifdef WLED_USE_AA_PIXELS
    inline void setPixelColorXY(float x, float y, uint32_t c, bool aa = true)     { setPixelColor(x, c, aa); }
    inline void setPixelColorXY(float x, float y, byte r, byte g, byte b, byte w = 0, bool aa = true) { setPixelColor(x, RGBW32(r,g,b,w), aa); }
    inline void setPixelColorXY(float x, float y, CRGB c, bool aa = true)         { setPixelColor(x, RGBW32(c.r,c.g,c.b,0), aa); }
    #endif
    inline uint32_t getPixelColorXY(int x, int y)                                 { return getPixelColor(x); }
    inline void blendPixelColorXY(uint16_t x, uint16_t y, uint32_t c, uint8_t blend) { blendPixelColor(x, c, blend); }
    inline void blendPixelColorXY(uint16_t x, uint16_t y, CRGB c, uint8_t blend)  { blendPixelColor(x, RGBW32(c.r,c.g,c.b,0), blend); }
    inline void addPixelColorXY(int x, int y, uint32_t color, bool fast = false)  { addPixelColor(x, color, fast); }
    inline void addPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0, bool fast = false) { addPixelColor(x, RGBW32(r,g,b,w), fast); }
    inline void addPixelColorXY(int x, int y, CRGB c, bool fast = false)          { addPixelColor(x, RGBW32(c.r,c.g,c.b,0), fast); }
    inline void fadePixelColorXY(uint16_t x, uint16_t y, uint8_t fade)            { fadePixelColor(x, fade); }
    inline void box_blur(unsigned i, bool vertical, fract8 blur_amount) {}
    inline void blur2D(uint8_t blur_amount, bool smear = false) {}
    inline void blurRow(uint32_t row, fract8 blur_amount, bool smear = false) {}
    inline void blurCol(uint32_t col, fract8 blur_amount, bool smear = false) {}
    inline void moveX(int8_t delta, bool wrap = false) {}
    inline void moveY(int8_t delta, bool wrap = false) {}
    inline void move(uint8_t dir, uint8_t delta, bool wrap = false) {}
    inline void drawCircle(uint16_t cx, uint16_t cy, uint8_t radius, uint32_t c, bool soft = false) {}
    inline void drawCircle(uint16_t cx, uint16_t cy, uint8_t radius, CRGB c, bool soft = false) {}
    inline void fillCircle(uint16_t cx, uint16_t cy, uint8_t radius, uint32_t c, bool soft = false) {}
    inline void fillCircle(uint16_t cx, uint16_t cy, uint8_t radius, CRGB c, bool soft = false) {}
    inline void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t c, bool soft = false) {}
    inline void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, CRGB c, bool soft = false) {}
    inline void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, uint32_t color, uint32_t = 0, int8_t = 0) {}
    inline void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, CRGB color) {}
    inline void drawCharacter(unsigned char chr, int16_t x, int16_t y, uint8_t w, uint8_t h, CRGB c, CRGB c2, int8_t rotate = 0) {}
    inline void wu_pixel(uint32_t x, uint32_t y, CRGB c) {}
  #endif
} segment;
//static int segSize = sizeof(Segment);

// main "strip" class
class WS2812FX {  // 96 bytes
  typedef uint16_t (*mode_ptr)(); // pointer to mode function
  typedef void (*show_callback)(); // pre show callback
  typedef struct ModeData {
    uint8_t     _id;   // mode (effect) id
    mode_ptr    _fcn;  // mode (effect) function
    const char *_data; // mode (effect) name and its UI control data
    ModeData(uint8_t id, uint16_t (*fcn)(void), const char *data) : _id(id), _fcn(fcn), _data(data) {}
  } mode_data_t;

  static WS2812FX* instance;

  public:

    WS2812FX() :
      paletteFade(0),
      paletteBlend(0),
      cctBlending(0),
      now(millis()),
      timebase(0),
      isMatrix(false),
#ifndef WLED_DISABLE_2D
      panels(1),
#endif
#ifdef WLED_AUTOSEGMENTS
      autoSegments(true),
#else
      autoSegments(false),
#endif
      correctWB(false),
      cctFromRgb(false),
      // semi-private (just obscured) used in effect functions through macros
      _colors_t{0,0,0},
      _virtualSegmentLength(0),
      // true private variables
      _suspend(false),
      _length(DEFAULT_LED_COUNT),
      _brightness(DEFAULT_BRIGHTNESS),
      _transitionDur(750),
      _targetFps(WLED_FPS),
      _frametime(FRAMETIME_FIXED),
      _cumulativeFps(50 << FPS_CALC_SHIFT),
      _isServicing(false),
      _isOffRefreshRequired(false),
      _hasWhiteChannel(false),
      _triggered(false),
      _modeCount(MODE_COUNT),
      _callback(nullptr),
      customMappingTable(nullptr),
      customMappingSize(0),
      _lastShow(0),
      _segment_index(0),
      _mainSegment(0)
    {
      WS2812FX::instance = this;
      _mode.reserve(_modeCount);     // allocate memory to prevent initial fragmentation (does not increase size())
      _modeData.reserve(_modeCount); // allocate memory to prevent initial fragmentation (does not increase size())
      if (_mode.capacity() <= 1 || _modeData.capacity() <= 1) _modeCount = 1; // memory allocation failed only show Solid
      else setupEffectData();
    }

    ~WS2812FX() {
      if (customMappingTable) delete[] customMappingTable;
      _mode.clear();
      _modeData.clear();
      _segments.clear();
#ifndef WLED_DISABLE_2D
      panel.clear();
#endif
      customPalettes.clear();
    }

    static WS2812FX* getInstance() { return instance; }

    void
#ifdef WLED_DEBUG
      printSize(),                                // prints memory usage for strip components
#endif
      finalizeInit(),                             // initialises strip components
      service(),                                  // executes effect functions when due and calls strip.show()
      setMode(uint8_t segid, uint8_t m),          // sets effect/mode for given segment (high level API)
      setColor(uint8_t slot, uint32_t c),         // sets color (in slot) for given segment (high level API)
      setCCT(uint16_t k),                         // sets global CCT (either in relative 0-255 value or in K)
      setBrightness(uint8_t b, bool direct = false),    // sets strip brightness
      setRange(uint16_t i, uint16_t i2, uint32_t col),  // used for clock overlay
      purgeSegments(),                            // removes inactive segments from RAM (may incure penalty and memory fragmentation but reduces vector footprint)
      setSegment(uint8_t n, uint16_t start, uint16_t stop, uint8_t grouping = 1, uint8_t spacing = 0, uint16_t offset = UINT16_MAX, uint16_t startY=0, uint16_t stopY=1),
      setMainSegmentId(uint8_t n),
      resetSegments(),                            // marks all segments for reset
      makeAutoSegments(bool forceReset = false),  // will create segments based on configured outputs
      fixInvalidSegments(),                       // fixes incorrect segment configuration
      setPixelColor(unsigned n, uint32_t c),      // paints absolute strip pixel with index n and color c
      show(),                                     // initiates LED output
      setTargetFps(uint8_t fps),
      setupEffectData();                          // add default effects to the list; defined in FX.cpp

    inline void resetTimebase()           { timebase = 0UL - millis(); }
    inline void restartRuntime()          { for (Segment &seg : _segments) { seg.markForReset().resetIfRequired(); } }
    inline void setTransitionMode(bool t) { for (Segment &seg : _segments) seg.startTransition(t ? _transitionDur : 0); }
    inline void setColor(uint8_t slot, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0)    { setColor(slot, RGBW32(r,g,b,w)); }
    inline void setPixelColor(unsigned n, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) { setPixelColor(n, RGBW32(r,g,b,w)); }
    inline void setPixelColor(unsigned n, CRGB c)                                         { setPixelColor(n, c.red, c.green, c.blue); }
    inline void fill(uint32_t c)          { for (unsigned i = 0; i < getLengthTotal(); i++) setPixelColor(i, c); } // fill whole strip with color (inline)
    inline void trigger()                                     { _triggered = true; }  // Forces the next frame to be computed on all active segments.
    inline void setShowCallback(show_callback cb)             { _callback = cb; }
    inline void setTransition(uint16_t t)                     { _transitionDur = t; } // sets transition time (in ms)
    inline void appendSegment(const Segment &seg = Segment()) { if (_segments.size() < getMaxSegments()) _segments.push_back(seg); }
    inline void suspend()                                     { _suspend = true; }    // will suspend (and canacel) strip.service() execution
    inline void resume()                                      { _suspend = false; }   // will resume strip.service() execution

    bool
      paletteFade,
      checkSegmentAlignment(),
      hasRGBWBus() const,
      hasCCTBus() const,
      isUpdating() const, // return true if the strip is being sent pixel updates
      deserializeMap(uint8_t n=0);

    inline bool isServicing() const          { return _isServicing; }           // returns true if strip.service() is executing
    inline bool hasWhiteChannel() const      { return _hasWhiteChannel; }       // returns true if strip contains separate white chanel
    inline bool isOffRefreshRequired() const { return _isOffRefreshRequired; }  // returns true if strip requires regular updates (i.e. TM1814 chipset)
    inline bool isSuspended() const          { return _suspend; }               // returns true if strip.service() execution is suspended
    inline bool needsUpdate() const          { return _triggered; }             // returns true if strip received a trigger() request

    uint8_t
      paletteBlend,
      cctBlending,
      getActiveSegmentsNum() const,
      getFirstSelectedSegId() const,
      getLastActiveSegmentId() const,
      getActiveSegsLightCapabilities(bool selectedOnly = false) const,
      addEffect(uint8_t id, mode_ptr mode_fn, const char *mode_name);         // add effect to the list; defined in FX.cpp;

    inline uint8_t getBrightness() const    { return _brightness; }       // returns current strip brightness
    inline uint8_t getMaxSegments() const   { return MAX_NUM_SEGMENTS; }  // returns maximum number of supported segments (fixed value)
    inline uint8_t getSegmentsNum() const   { return _segments.size(); }  // returns currently present segments
    inline uint8_t getCurrSegmentId() const { return _segment_index; }    // returns current segment index (only valid while strip.isServicing())
    inline uint8_t getMainSegmentId() const { return _mainSegment; }      // returns main segment index
    inline uint8_t getPaletteCount() const  { return 13 + GRADIENT_PALETTE_COUNT + customPalettes.size(); }
    inline uint8_t getTargetFps() const     { return _targetFps; }        // returns rough FPS value for las 2s interval
    inline uint8_t getModeCount() const     { return _modeCount; }        // returns number of registered modes/effects

    uint16_t
      getLengthPhysical() const,
      getLengthTotal() const, // will include virtual/nonexistent pixels in matrix
      getFps() const,
      getMappedPixelIndex(uint16_t index) const;

    inline uint16_t getFrameTime() const    { return _frametime; }        // returns amount of time a frame should take (in ms)
    inline uint16_t getMinShowDelay() const { return MIN_SHOW_DELAY; }    // returns minimum amount of time strip.service() can be delayed (constant)
    inline uint16_t getLength() const       { return _length; }           // returns actual amount of LEDs on a strip (2D matrix may have less LEDs than W*H)
    inline uint16_t getTransition() const   { return _transitionDur; }    // returns currently set transition time (in ms)

    unsigned long now, timebase;
    uint32_t getPixelColor(unsigned) const;

    inline uint32_t getLastShow() const       { return _lastShow; }           // returns millis() timestamp of last strip.show() call
    inline uint32_t segColor(uint8_t i) const { return _colors_t[i]; }        // returns currently valid color (for slot i) AKA SEGCOLOR(); may be blended between two colors while in transition

    const char *
      getModeData(uint8_t id = 0) const { return (id && id<_modeCount) ? _modeData[id] : PSTR("Solid"); }

    const char **
      getModeDataSrc() { return &(_modeData[0]); } // vectors use arrays for underlying data

    Segment&        getSegment(uint8_t id);
    inline Segment& getFirstSelectedSeg() { return _segments[getFirstSelectedSegId()]; }  // returns reference to first segment that is "selected"
    inline Segment& getMainSegment()      { return _segments[getMainSegmentId()]; }       // returns reference to main segment
    inline Segment* getSegments()         { return &(_segments[0]); }                     // returns pointer to segment vector structure (warning: use carefully)

  // 2D support (panels)
    bool
      isMatrix;

#ifndef WLED_DISABLE_2D
    #define WLED_MAX_PANELS 18
    uint8_t
      panels;

    typedef struct panel_t {
      uint16_t xOffset; // x offset relative to the top left of matrix in LEDs
      uint16_t yOffset; // y offset relative to the top left of matrix in LEDs
      uint8_t  width;   // width of the panel
      uint8_t  height;  // height of the panel
      union {
        uint8_t options;
        struct {
          bool bottomStart : 1; // starts at bottom?
          bool rightStart  : 1; // starts on right?
          bool vertical    : 1; // is vertical?
          bool serpentine  : 1; // is serpentine?
        };
      };
      panel_t()
        : xOffset(0)
        , yOffset(0)
        , width(8)
        , height(8)
        , options(0)
      {}
    } Panel;
    std::vector<Panel> panel;
#endif

    void setUpMatrix();     // sets up automatic matrix ledmap from panel configuration

    // outsmart the compiler :) by correctly overloading
    inline void setPixelColorXY(int x, int y, uint32_t c)   { setPixelColor((unsigned)(y * Segment::maxWidth + x), c); }
    inline void setPixelColorXY(int x, int y, byte r, byte g, byte b, byte w = 0) { setPixelColorXY(x, y, RGBW32(r,g,b,w)); }
    inline void setPixelColorXY(int x, int y, CRGB c)       { setPixelColorXY(x, y, RGBW32(c.r,c.g,c.b,0)); }

    inline uint32_t getPixelColorXY(int x, int y) const     { return getPixelColor(isMatrix ? y * Segment::maxWidth + x : x); }

  // end 2D support

    void loadCustomPalettes(); // loads custom palettes from JSON
    std::vector<CRGBPalette16> customPalettes; // TODO: move custom palettes out of WS2812FX class

    struct {
      bool autoSegments : 1;
      bool correctWB    : 1;
      bool cctFromRgb   : 1;
    };

    // using public variables to reduce code size increase due to inline function getSegment() (with bounds checking)
    // and color transitions
    uint32_t _colors_t[3]; // color used for effect (includes transition)
    uint16_t _virtualSegmentLength;

    std::vector<segment> _segments;
    friend class Segment;

  private:
    volatile bool _suspend;

    uint16_t _length;
    uint8_t  _brightness;
    uint16_t _transitionDur;

    uint8_t  _targetFps;
    uint16_t _frametime;
    uint16_t _cumulativeFps;

    // will require only 1 byte
    struct {
      bool _isServicing          : 1;
      bool _isOffRefreshRequired : 1; //periodic refresh is required for the strip to remain off.
      bool _hasWhiteChannel      : 1;
      bool _triggered            : 1;
    };

    uint8_t                  _modeCount;
    std::vector<mode_ptr>    _mode;     // SRAM footprint: 4 bytes per element
    std::vector<const char*> _modeData; // mode (effect) name and its slider control data array

    show_callback _callback;

    uint16_t* customMappingTable;
    uint16_t  customMappingSize;

    unsigned long _lastShow;

    uint8_t _segment_index;
    uint8_t _mainSegment;
};

extern const char JSON_mode_names[];
extern const char JSON_palette_names[];

#endif

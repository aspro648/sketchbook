#include "SPI.h"
#include "LPD8806.h"
#include "TimerOne.h"

#define MAX_SPARKS 20
#define SPI true
#define MIN_TRIGGER 10
#define MAX_V ((uint32_t)160 * 256)  // steps/tick * 256

#define MAX_SPARKBURST 10
#define MIN_SPARKBURST 2
#define SPARKBURST_SLOWEST 3000LL
#define SPARKBURST_FASTEST 20LL

// SPI: data = D11, clock = D13.
// Attach button between pins D7 and ground.

#if !SPI
const int DATA_PIN = 2;
const int CLOCK_PIN = 3;
#endif
const int BUTTON_PIN = 7;
const int PIXEL_COUNT = 64 - 22;
const int UPDATE_MS = 10;
const uint32_t IDLE_TIMEOUT = 30000;    // milliseconds

typedef struct spark {
    uint16_t pos;
    int16_t v;
    uint8_t r, g, b;
} spark;

spark sparks[MAX_SPARKS];
spark *end_sparks = sparks;
uint8_t spark_count = 0;
uint32_t idle_timeout = 0;

LPD8806 strip;

// hsv2rgb from advancedLEDbeltKit.
// Fixed-point colorspace conversion: HSV (hue-saturation-value) to RGB.
// This is a bit like the 'Wheel' function from the original strandtest
// code on steroids.  The angular units for the hue parameter may seem a
// bit odd: there are 1536 increments around the full color wheel here --
// not degrees, radians, gradians or any other conventional unit I'm
// aware of.  These units make the conversion code simpler/faster, because
// the wheel can be divided into six sections of 256 values each, very
// easy to handle on an 8-bit microcontroller.  Math is math, and the
// rendering code elsehwere in this file was written to be aware of these
// units.  Saturation and value (brightness) range from 0 to 255.
long hsv2rgb(long h, byte s, byte v)
{
  byte r, g, b, lo;
  int  s1;
  long v1;

  // Hue
  h %= 1536;           // -1535 to +1535
  if(h < 0) h += 1536; //     0 to +1535
  lo = h & 255;        // Low byte  = primary/secondary color mix
  switch(h >> 8) {     // High byte = sextant of colorwheel
    case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
    case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
    case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
    case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
    case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
    default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
  }

  // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
  // on the result rather than a costly divide, while the type upgrade to int
  // avoids repeated type conversions in both directions.
  s1 = s + 1;
  r = 255 - (((255 - r) * s1) >> 8);
  g = 255 - (((255 - g) * s1) >> 8);
  b = 255 - (((255 - b) * s1) >> 8);

  // Value (brightness) and 24-bit color concat merged: similar to above, add
  // 1 to allow shifts, and upgrade to long makes other conversions implicit.
  v1 = v + 1;
  return (((r * v1) & 0xff00) << 8) |
          ((g * v1) & 0xff00)       |
         ( (b * v1)           >> 8);
}

void generate_random_color(uint8_t *rp, uint8_t *gp, uint8_t *bp)
{
    long h = random(1536);
    uint8_t s = 255;
    uint8_t v = 255;
    long rgb = hsv2rgb(h, s, v);
    *rp = rgb >> 16;
    *gp = rgb >> 8;
    *bp = rgb;
}


void advance_sparks(void)
{
    for (spark *sp = sparks; sp < end_sparks; sp++) {
        uint16_t new_pos = sp->pos + sp->v;
        bool bounced = false;
        if (sp->v < 0 && (uint16_t)-sp->v > sp->pos) {
            // Bounce off the low end
            new_pos = ~new_pos + 1;  // negation for nerds
            bounced = true;
        } else if (new_pos >= PIXEL_COUNT << 8) {
            // Bounce off the high end
            new_pos = (PIXEL_COUNT << 9) - new_pos;
            bounced = true;
        }
        if (bounced) {
            sp->v = -sp->v;
            sp->r = sp->r >> 1 | sp->r >> 2;
            sp->g = sp->g >> 1 | sp->g >> 2;
            sp->b = sp->b >> 1 | sp->b >> 2;
        }
        sp->pos = new_pos;
    }
}

void light_sparks(void)
{
    for (spark *sp = sparks; sp < end_sparks; sp++) {
        uint32_t c = strip.getPixelColor(sp->pos >> 8);
        uint8_t r = c >> 8, g = c >> 16, b = c >> 0;
        r = min(127, r + sp->r);
        g = min(127, g + sp->g);
        b = min(127, b + sp->b);
        strip.setPixelColor(sp->pos >> 8, r, g, b);
    }
}

void ignite_spark(uint32_t trigger_length)
{
    uint16_t pos = 0;
    int16_t v = MAX_V / trigger_length;
    uint8_t r, g, b;
    generate_random_color(&r, &g, &b);
    uint8_t saved_sreg = SREG; 
    cli();
    {
        if (end_sparks < sparks + MAX_SPARKS) {
            spark *sp = end_sparks++;
            spark_count++;
            sp->pos = pos;
            sp->v = v;
            sp->r = r;
            sp->g = g;
            sp->b = b;
        }
    }
    SREG = saved_sreg;
}

void extinguish_sparks(void)
{
    for (spark *sp = sparks; sp < end_sparks; ) {
        strip.setPixelColor(sp->pos >> 8, 0, 0, 0);
        if (sp->r || sp->g || sp->g)
            sp++;
        else {
            *sp = *--end_sparks;
            if (!--spark_count)
                idle_timeout = millis() + IDLE_TIMEOUT;
        }
    }
}

void callback()
{
    strip.show();
    extinguish_sparks();
    advance_sparks();
    light_sparks();
}

void wakeup_animation()
{
    uint8_t r, g, b;
    generate_random_color(&r, &g, &b);
    uint32_t t0 = millis();
    for (int i = 0; i < PIXEL_COUNT; i++)
        strip.setPixelColor(i, r, g, b);
    strip.show();
    while (millis() - t0 < 300)
        continue;
    for (int i = 0; i < PIXEL_COUNT; i++)
        strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    while (millis() - t0 < 500)
        continue;
    generate_random_color(&r, &g, &b);
    for (int i = 0; i < PIXEL_COUNT; i++)
        strip.setPixelColor(i, r, g, b);
    strip.show();
    while (millis() - t0 < 800)
        continue;
    for (int i = 0; i < PIXEL_COUNT; i++)
        strip.setPixelColor(i, 0, 0, 0);      
    strip.show();
}

void sparkburst(void)
{
    int n_sparks = random(MAX_SPARKBURST - MIN_SPARKBURST) + MIN_SPARKBURST;
    for (int i = 0; i < n_sparks; i++) {
        const uint32_t s = SPARKBURST_SLOWEST;
        const uint32_t f = SPARKBURST_FASTEST;
        
        uint32_t spark_speed = random(s - f) + f;
        ignite_spark(spark_speed);
    }
}

void setup(void)
{
    long seed = 0;
    for (int i = 0; i < 6; i++)
        seed = seed << 3 | analogRead(i);
    randomSeed(seed);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    strip.updateLength(PIXEL_COUNT);
#if !SPI
    strip.updatePins(DATA_PIN, CLOCK_PIN);
#endif
    strip.begin();

//    ignite_spark(25);
//    ignite_spark(66);
//    ignite_spark(99);
    wakeup_animation();

    Timer1.initialize();
    Timer1.attachInterrupt(callback, UPDATE_MS * 1000);
    idle_timeout = millis() + IDLE_TIMEOUT;
}

void loop(void)
{
    // check button state.
    // if was up, is down, record start time.
    // if was down, is up, record end time; create new spark.
    static bool was_pressed = false;
    static uint32_t time_down = 0;

    bool is_pressed = digitalRead(BUTTON_PIN) == LOW;
    if (is_pressed && !was_pressed) {
        // Button Down
        time_down = millis();
        idle_timeout = 0;
    }
    if (!is_pressed && was_pressed) {
        uint32_t time_up = millis();
        uint32_t trigger_length = time_up - time_down;
        if (trigger_length >= MIN_TRIGGER)
            ignite_spark(time_up - time_down);
    }
    was_pressed = is_pressed;
    
    if (idle_timeout && millis() >= idle_timeout) {
        idle_timeout = 0;
        wakeup_animation();
        sparkburst();
    }
}

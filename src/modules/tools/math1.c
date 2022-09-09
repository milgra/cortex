#ifndef math1_h
#define math1_h

#include <math.h>
#include <stdio.h>

float float_tween_linear(float start, float delta, float time, float duration);
float float_tween_ease_in_quadratic(float start, float delta, float time, float duration);
float float_tween_ease_out_quadratic(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_quadratic(float start, float delta, float time, float duration);
float float_tween_ease_in_cubic(float start, float delta, float time, float duration);
float float_tween_ease_out_cubic(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_cubic(float start, float delta, float time, float duration);
float float_tween_ease_in_quartic(float start, float delta, float time, float duration);
float float_tween_ease_out_quartic(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_quartic(float start, float delta, float time, float duration);
float float_tween_ease_in_quintic(float start, float delta, float time, float duration);
float float_tween_ease_out_quintic(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_quintic(float start, float delta, float time, float duration);
float float_tween_ease_in_sine(float start, float delta, float time, float duration);
float float_tween_ease_out_sine(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_sine(float start, float delta, float time, float duration);
float float_tween_ease_in_exp(float start, float delta, float time, float duration);
float float_tween_ease_out_exp(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_exp(float start, float delta, float time, float duration);
float float_tween_ease_in_circ(float start, float delta, float time, float duration);
float float_tween_ease_out_circ(float start, float delta, float time, float duration);
float float_tween_ease_in_ease_out_circ(float start, float delta, float time, float duration);

#endif

#if __INCLUDE_LEVEL__ == 0

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
    #define M_PI_2 3.14159265358979323846 * 2
#endif

/* simple linear tweening - no easing, no acceleration */

float float_tween_linear(float start, float delta, float time, float duration)
{
    return delta * time / duration + start;
}

/* quadratic easing in - accelerating from zero velocity */

float float_tween_ease_in_quadratic(float start, float delta, float time, float duration)
{
    time /= duration;
    return delta * time * time + start;
}

/* quadratic easing out - decelerating to zero velocity */

float float_tween_ease_out_quadratic(float start, float delta, float time, float duration)
{
    time /= duration;
    return -delta * time * (time - 2.0) + start;
}

/* quadratic easing in/out - acceleration until halfway, then deceleration */

float float_tween_ease_in_ease_out_quadratic(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return delta / 2.0 * time * time + start;
    time -= 1.0;
    return -delta / 2.0 * (time * (time - 2.0) - 1.0) + start;
}

/* cubic easing in - accelerating from zero velocity */

float float_tween_ease_in_cubic(float start, float delta, float time, float duration)
{
    time /= duration;
    return delta * time * time * time + start;
}

/* cubic easing out - decelerating to zero velocity */

float float_tween_ease_out_cubic(float start, float delta, float time, float duration)
{
    time /= duration;
    time -= 1.0;
    return delta * (time * time * time + 1.0) + start;
}

/* cubic easing in/out - acceleration until halfway, then deceleration */

float float_tween_ease_in_ease_out_cubic(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return delta / 2.0 * time * time * time + start;
    time -= 2.0;
    return delta / 2.0 * (time * time * time + 2.0) + start;
}

/* quartic easing in - accelerating from zero velocity */

float float_tween_ease_in_quartic(float start, float delta, float time, float duration)
{
    time /= duration;
    return delta * time * time * time * time + start;
}

/* quartic easing out - decelerating to zero velocity */

float float_tween_ease_out_quartic(float start, float delta, float time, float duration)
{
    time /= duration;
    time -= 1.0;
    return -delta * (time * time * time * time - 1.0) + start;
}

/* quartic easing in/out - acceleration until halfway, then deceleration */

float float_tween_ease_in_ease_out_quartic(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return delta / 2.0 * time * time * time * time + start;
    time -= 2.0;
    return -delta / 2.0 * (time * time * time * time - 2.0) + start;
}

/* quintic easing in - accelerating from zero velocity */

float float_tween_ease_in_quintic(float start, float delta, float time, float duration)
{
    time /= duration;
    return delta * time * time * time * time * time + start;
}

/* quintic easing out - decelerating to zero velocity */

float float_tween_ease_out_quintic(float start, float delta, float time, float duration)
{
    time /= duration;
    time -= 1.0;
    return delta * (time * time * time * time * time + 1.0) + start;
}

/* quintic easing in/out - acceleration until halfway, then deceleration */

float float_tween_ease_in_ease_out_quintic(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return delta / 2.0 * time * time * time * time * time + start;
    time -= 2.0;
    return delta / 2.0 * (time * time * time * time * time + 2.0) + start;
}

/* sinusoidal easing in - accelerating from zero velocity */

float float_tween_ease_in_sine(float start, float delta, float time, float duration)
{
    return -delta * cosf(time / duration * M_PI_2) + delta + start;
}

/* sinusoidal easing out - decelerating to zero velocity */

float float_tween_ease_out_sine(float start, float delta, float time, float duration)
{
    return delta * sinf(time / duration * M_PI_2) + start;
}

/* sinusoidal easing in/out - accelerating until halfway, then decelerating */

float float_tween_ease_in_ease_out_sine(float start, float delta, float time, float duration)
{
    return -delta / 2.0 * (cosf(M_PI * time / duration) - 1.0) + start;
}

/* exponential easing in - accelerating from zero velocity */

float float_tween_ease_in_exp(float start, float delta, float time, float duration)
{
    return delta * powf(2, 10 * (time / duration - 1.0)) + start;
}

/* exponential easing out - decelerating to zero velocity */

float float_tween_ease_out_exp(float start, float delta, float time, float duration)
{
    return delta * (-powf(2, -10.0 * time / duration) + 1.0) + start;
}

/* exponential easing in/out - accelerating until halfway, then decelerating */

float float_tween_ease_in_ease_out_exp(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return delta / 2.0 * powf(2, 10.0 * (time - 1.0)) + start;
    time -= 1.0;
    return delta / 2.0 * (-powf(2, -10.0 * time) + 2.0) + start;
}

/* circular easing in - accelerating from zero velocity */

float float_tween_ease_in_circ(float start, float delta, float time, float duration)
{
    time /= duration;
    return -delta * (sqrtf(1.0 - time * time) - 1.0) + start;
}

/* circular easing out - decelerating to zero velocity */

float float_tween_ease_out_circ(float start, float delta, float time, float duration)
{
    time /= duration;
    time -= 1.0;
    return delta * sqrtf(1.0 - time * time) + start;
}

/* circular easing in/out - accelerating until halfway, then decelerating */

float float_tween_ease_in_ease_out_circ(float start, float delta, float time, float duration)
{
    time /= duration / 2.0;
    if (time < 1.0) return -delta / 2.0 * (sqrtf(1.0 - time * time) - 1.0) + start;
    time -= 2.0;
    return delta / 2.0 * (sqrtf(1.0 - time * time) + 1.0) + start;
}

#endif

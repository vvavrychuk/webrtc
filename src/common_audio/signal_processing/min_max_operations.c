/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 * This file contains the implementation of functions
 * WebRtcSpl_MaxAbsValueW16()
 * WebRtcSpl_MaxAbsValueW32()
 * WebRtcSpl_MaxValueW16()
 * WebRtcSpl_MaxValueW32()
 * WebRtcSpl_MinValueW16()
 * WebRtcSpl_MinValueW32()
 * WebRtcSpl_MaxAbsIndexW16()
 * WebRtcSpl_MaxIndexW16()
 * WebRtcSpl_MaxIndexW32()
 * WebRtcSpl_MinIndexW16()
 * WebRtcSpl_MinIndexW32()
 *
 */

#include "signal_processing_library.h"

#include <stdlib.h>

// TODO(bjorn/kma): Consolidate function pairs (e.g. combine
// WebRtcSpl_MaxAbsValueW16 and WebRtcSpl_MaxAbsIndexW16 into a single one.)

#if !(defined(WEBRTC_ANDROID) && defined(WEBRTC_ARCH_ARM_NEON))

// Maximum absolute value of word16 vector.
int16_t WebRtcSpl_MaxAbsValueW16(const int16_t* vector, int length) {
  int i = 0, absolute = 0, maximum = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);

    if (absolute > maximum) {
      maximum = absolute;
    }
  }

  // Guard the case for abs(-32768).
  if (maximum > WEBRTC_SPL_WORD16_MAX) {
    maximum = WEBRTC_SPL_WORD16_MAX;
  }

  return (int16_t)maximum;
}

// Maximum absolute value of word32 vector.
int32_t WebRtcSpl_MaxAbsValueW32(const int32_t* vector, int length) {
  // Use uint for the local variables, to accommodate the value
  // of abs(0x80000000).

  uint absolute = 0, maximum = 0;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);
    if (absolute > maximum) {
      maximum = absolute;
    }
  }

  maximum = WEBRTC_SPL_MIN(maximum, WEBRTC_SPL_WORD32_MAX);

  return (int32_t)maximum;
}

// Maximum value of word16 vector.
#ifndef XSCALE_OPT
int16_t WebRtcSpl_MaxValueW16(const int16_t* vector, int length) {
  int16_t maximum = WEBRTC_SPL_WORD16_MIN;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return maximum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum)
      maximum = vector[i];
  }
  return maximum;
}

// Maximum value of word32 vector.
int32_t WebRtcSpl_MaxValueW32(const int32_t* vector, int length) {
  int32_t maximum = WEBRTC_SPL_WORD32_MIN;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return maximum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum)
      maximum = vector[i];
  }
  return maximum;
}
#else
#pragma message(">> WebRtcSpl_MaxValueW16 is excluded from this build")
#pragma message(">> WebRtcSpl_MaxValueW32 is excluded from this build")
#endif

// Minimum value of word16 vector.
int16_t WebRtcSpl_MinValueW16(const int16_t* vector, int length) {
  int16_t minimum = WEBRTC_SPL_WORD16_MAX;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return minimum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum)
      minimum = vector[i];
  }
  return minimum;
}

// Minimum value of word32 vector.
int32_t WebRtcSpl_MinValueW32(const int32_t* vector, int length) {
  int32_t minimum = WEBRTC_SPL_WORD32_MAX;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return minimum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum)
      minimum = vector[i];
  }
  return minimum;
}
#endif


// Index of maximum absolute value in a word16 vector.
int WebRtcSpl_MaxAbsIndexW16(const int16_t* vector, int length) {
  // Use type int for local variables, to accomodate the value of abs(-32768).

  int i = 0, absolute = 0, maximum = 0, index = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);

    if (absolute > maximum) {
      maximum = absolute;
      index = i;
    }
  }

  return index;
}

// Index of maximum value in a word16 vector.
int WebRtcSpl_MaxIndexW16(const int16_t* vector, int length) {
  int i = 0, index = 0;
  int16_t maximum = WEBRTC_SPL_WORD16_MIN;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum) {
      maximum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of maximum value in a word32 vector.
int WebRtcSpl_MaxIndexW32(const int32_t* vector, int length) {
  int i = 0, index = 0;
  int32_t maximum = WEBRTC_SPL_WORD32_MIN;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum) {
      maximum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of minimum value in a word16 vector.
#ifndef XSCALE_OPT
int WebRtcSpl_MinIndexW16(const int16_t* vector, int length) {
  int i = 0, index = 0;
  int16_t minimum = WEBRTC_SPL_WORD16_MAX;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum) {
      minimum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of minimum value in a word32 vector.
int WebRtcSpl_MinIndexW32(const int32_t* vector, int length) {
  int i = 0, index = 0;
  int32_t minimum = WEBRTC_SPL_WORD32_MAX;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum) {
      minimum = vector[i];
      index = i;
    }
  }

  return index;
}

#else
#pragma message(">> WebRtcSpl_MinIndexW16 is excluded from this build")
#pragma message(">> WebRtcSpl_MinIndexW32 is excluded from this build")
#endif

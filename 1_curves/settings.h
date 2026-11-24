#ifndef SETTINGS_H
#define SETTINGS_H

#include "shadertypes.h"

/**
 * Struct that contains all the settings of the program. Initialised with a
 * number of default values.
 */
typedef struct Settings {
  bool showNet = true;
  bool showSubdivisionCurve = false;
  int selectedPoint = -1;

  ShaderType currentShader = ShaderType::FLAT;

} Settings;

#endif  // SETTINGS_H

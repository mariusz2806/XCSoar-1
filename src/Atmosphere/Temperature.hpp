/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_ATMOSPHERE_TEMPERATURE_HPP
#define XCSOAR_ATMOSPHERE_TEMPERATURE_HPP

#include "Math/fixed.hpp"
#include "Compiler.h"

/**
 * The offset between 0 Kelvin and 0 degrees Celsius [K].
 */
static gcc_constexpr_data fixed CELSIUS_OFFSET(273.15);

/**
 * Convert a temperature from Kelvin to degrees Celsius.
 */
gcc_constexpr_function
static inline fixed
KelvinToCelsius(fixed kelvin)
{
  return kelvin - CELSIUS_OFFSET;
}

/**
 * Convert a temperature from degrees Celsius to Kelvin.
 */
gcc_constexpr_function
static inline fixed
CelsiusToKelvin(fixed celsius)
{
  return celsius + CELSIUS_OFFSET;
}

#endif
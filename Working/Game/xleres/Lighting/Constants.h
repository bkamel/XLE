// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#if !defined(LIGHTING_CONSTANTS_H)
#define LIGHTING_CONSTANTS_H

//
// This file contains some fixed constants used in the lighting pipeline
// Some of these constants define the meaning of other constants.
// For example, RefractiveIndexMin/Max define the range of the "Specular"
// material parameter. Changing those values will actually change the meaning
// of "specular" in every material.
//
// Different project might want different values for RefractiveIndexMin/Max
// but the values should be constant and universal within a single project
//

static const float RefractiveIndexMin = 1.0f;
static const float RefractiveIndexMax = 1.8f;
static const float MetalReflectionBoost = 4.f; // 300.f;
static const float ReflectionBlurrinessFromRoughness = 5.f;

// We can choose 2 options for defining the "specular" parameter
//  a) Linear against "refractive index"
//      this is more expensive, and tends to mean that the most
//      useful values of "specular" are clustered around 0 (where there
//      is limited fraction going on)
//  b) Linear against "F0"
//      a little cheaper, and more direct. There is a linear relationship
//      between the "specular" parameter and the brightness of the
//      "center" part of the reflection. So it's easy to understand.
//  Also note that we have limited precision for the specular parameter, if
//  we're reading it from texture maps... So it seems to make sense to make
//  it linear against F0.
#define SPECULAR_LINEAR_AGAINST_F0 1

#endif
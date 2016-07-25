/*
Copyright DaiMysha (c) 2015, All rights reserved.
DaiMysha@gmail.com
https://github.com/DaiMysha

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef HEADER_DMGDVT_STATICDATA
#define HEADER_DMGDVT_STATICDATA

namespace dm {
namespace ls {
namespace StaticData {
    const char* LIGHT_ATTENUATION_SHADER = "uniform vec2 center;uniform float radius;uniform vec4 color;uniform float bleed;uniform float linearFactor;uniform bool iso;void main() {vec2 pixel = gl_FragCoord.xy;float dist = length(center - pixel);float distFromFalloff = radius - dist;float attenuation = 0.0;attenuation = distFromFalloff * (bleed / (dist*dist) + linearFactor / radius);attenuation = clamp(attenuation, 0.0, 1.0);vec4 color = vec4(attenuation, attenuation, attenuation, 1.0) * vec4(color.r, color.g, color.b, color.a);gl_FragColor = color;}";
}
}
}

#endif // HEADER_DMGDVT_STATICDATA



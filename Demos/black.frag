#version 440
 layout(location = 0) in vec2 coord;
 layout(location = 0) out vec4 fragColor;
 layout(std140, binding = 0) uniform buf {
     mat4 qt_Matrix;
     float qt_Opacity;
 };
 layout(binding = 1) uniform sampler2D src;
 void main() {
     vec4 tex = texture(src, coord);
     vec4 newColor = vec4(tex.rgb, 1.0); // set the alpha channel to 1 for all colors except black
     if (tex == vec4(0.0, 0.0, 0.0, 1.0)) {
         newColor.a = 0.0; // set the alpha channel to 0 for black color
     }
     fragColor = newColor * qt_Opacity;
}

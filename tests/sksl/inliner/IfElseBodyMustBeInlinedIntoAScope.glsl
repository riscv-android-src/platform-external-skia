
out vec4 sk_FragColor;
uniform vec4 colorGreen;
uniform vec4 colorRed;
vec4 main() {
    vec4 c = colorRed;
    if (colorGreen.y == 0.0) ; else {
        vec4 _0_x = colorGreen;
        c = _0_x;
    }
    return c;
}

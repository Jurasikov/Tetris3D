#version 330

uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 n;
in vec4 l;
in vec4 l1;
in vec4 v;
in vec2 iTexCoord0;

void main(void) {

	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 ml1 = normalize(l1);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);
	vec4 mr1 = reflect(-ml1, mn);

	//Parametry powierzchni
	vec4 kd = texture(textureMap0,iTexCoord0);
	vec4 ks = kd;

	//Obliczenie modelu oświetlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float nl1 = clamp(dot(mn, ml1), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 50);
	float rv1 = pow(clamp(dot(mr1, mv), 0, 1), 50);
	pixelColor= vec4(kd.rgb * 0.33, kd.a) + vec4(kd.rgb * 0.66 * nl + kd.rgb * 0.66 * nl1, kd.a) + vec4(ks.rgb*rv + ks.rgb*rv1, 0);
}

#version 330

smooth in vec3 N;
smooth in vec3 V;

flat in vec3 light_pos;

out vec4 frag_color;

uniform vec3 in_color;

void main (void) {
   vec3 L = normalize(light_pos - V);
   vec3 E = normalize(-V);
   vec3 R = normalize(-reflect(L, N));

   //calculate Ambient Term:
   vec3 Iamb = in_color;

   //calculate Diffuse Term:
   vec3 Idiff = vec3(0.8, 0.8, 1.0) * vec3(max(dot(N, L), 0.0));

   // calculate Specular Term:
   vec3 Ispec = vec3(0.1, 0.1, 1.0) * pow(max(dot(R, E), 0.0), 2.2);
   Ispec = clamp(Ispec, 0.0, 1.0);

   // write Total Color:
   frag_color = vec4(Iamb * 0.4 + Idiff * 0.6 + Ispec * 0.2, 1.0);
   //frag_color = vec4(243.0/256, 123.0/256, 173.0/256, 1.0);
     //frag_color=vec4(V,1.0);
}


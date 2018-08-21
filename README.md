# Procedural-Terrain-Estimator
generate procedural terrains by specific data and algorithm (soon will be decided) and estimate them with tensorflow. with it's meta score, choose highest one and render it using OpenGL.

## GLSLCACHE

this is **offline GLSL compiler**. in command line, just type below command.
compile given shader's paths and **provide compiled program binary output**.


```

glslcache vert.glsl frag.glsl

//if you want to set output name, append -o option.
glslcache vert.glsl frag.glsl -o newname.glslcache

//if you use tessellation, just append -t option.
glslcache vert.glsl frag.glsl -t tcs.glsl tes.glsl -o newname.glslcache

//if you use geometry shader, just append -g option.
glslcache vert.glsl frag.glsl -t tcs.glsl tes.glsl -g gs.glsl -o newname.glslcache
```

## RENDERER
this is for **rendering procedural terrain**. default supported platform is Window. other platform like Linux, OS X will be supported later.
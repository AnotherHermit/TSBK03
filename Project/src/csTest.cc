// Create buffers for: 
// * A uint per bin (two bins for pingpong):  2 * 4 * num bins (256*256*256 = 16M) = 128MB
// * vec3 pos, vec3 vel, bin per particle (two arrays for pingpong): (3*4*2 + 4) * particles (2M) * 2 = 112MB
// 

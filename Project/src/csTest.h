//		Shader programs needed to perform flocking on GPU 

// 1.	Calculate bin and increase bin counters (Comp Shader) #DONE
//		Input: buffer with particles (1), cleared bin buffer (1)
//		Work: calculates particle bin and assigns it (totaly independent),
//			atomicAdd to the counter of the bin (partially independent)
//		Output: Bin in particles and updated bin buffer

// 2.	Prefix sum over all bins (Comp Shader or CPU) #DONEONCPU
//		Input: updated bin buffer
//		Output: prefix sum buffer (new buffer or reuse the same? pingpong?)

// 3.	Counting sort of all particles (Comp Shader) #DONE
//		Input: buffers with particles (2) one updated and one for writing,
//			Prefix sum buffer, cleared bin buffer
//		Work: Writes the particles in order based on their assigned bin from the sum buffer
//			updates the bin buffer to know how many has been written
//		Output: Sorted particle buffer, number of particles per bin

// 4.	Particle time integration (Comp Shader)		#STARTED
//		Input: output particles buffer, sorted particles buffer, prefix sum, particles per bin, time
//		Work: reads continuoulsy the buffers in order which are within the particles neighbour radius
//			specified in the prefix sum adding together particles per bin for bin that are in order.
//			Performs the flocking calculations per read other particle and updates the output.
//		Output: Updated particles 

// 5.	Cull particles (Comp Shader)	#DONE 
//		Input: Updated particles, culled particles buffer, atomicCounter for number remaining
//			Normals and a point for each frustum plane and the particle radius
//		Work: Calculates if a particle has some part of the surface within the frustum
//			otherwise it is not put in the output buffer.
//		Output: Counter of the number of remaining particles, buffer of particle positions
//		Idea: Cull the grid instead in a first pass and then only output the particles that belong to
//			the non-culled grids.
//		Idea: Cull the particles as they are about to be drawn (drop them in geometry shader)
//			this means no data transfer back to CPU.

// 6.	Draw particles (Billboards) (Vert+Geom+Frag Shader)	#DONE
//		Input: Vertex array of points, radius
//		Work: Geometry shader creates billboards that is textured with a circle

// 6.	Draw particles (Models) (Vert+Geom Shader) #DONE
//		Input: Model, vertex array of points
//		Work: Instanced rendering of sphere model.



// Needed buffers: 											Size:
// Bins:													256*256*256 = 16M
//		* buffer with amout of particles in each bin (1)	16M * 4B = 64MB
//		* buffer for prefix sum (1-2 algorithm dependent)	16M * 4B = 64MB
//															= 128MB
// Particles:												8M
//		* buffer for particle data (2)						8M * (3 + 3 + 1) * 4B * 2 = 448MB
//		* buffer for culled particle positions (1)			8M * 3 * 4B = 96MB
//															= 544MB 
// Counters													~= 0
//		* Atomic counter for culling (1 int)
//															= 672MB

#pragma once
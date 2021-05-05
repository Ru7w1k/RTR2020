==================================
    CLOTH RENDERING USING CUDA
==================================


Description:
-----------------
The cloth is formed using a 2D mesh of particle mass 
and attached springs in between them.
Force of gravity acts on all of those particles.
There is one external force (wind) that can be applied by the user.
Using Hooke's law and all other forces a resultant force is calculated
for each particle, then the velocity of each particle
is calculated and the next position is determined using that.
This process is repeated for all particles and for 800 times per frame.
After that, the normals are calculated for that mesh, for lighting calculations.

It also contains a simplified version of collision detection. So
while falling down, it can detect if there are any objects and the
the cloth will deform according to that.

The calculations are implemented on both the CPU and GPU side, and
user can toggle between them at any time using keys C and G.


Scenes:
-----------
1: Curtain
2: Cloth falling on 5 spheres
3: Cloth falling on the table surface

The program responds to:
------------------------
W: toggle wind
1,2,3: switch to the scene
R: reset the scene
Left-Right arrow: move the camera to left or right
G: switch to GPU(CUDA)
C: switch to CPU

Reference:
---------------
1. CUDA by Example
   - Jason Sanders, Edward Kandrot

2. OpenGL Superbible (7th)
   - Graham Sellers, Richard S. Wright(Jr.), Nicholas Haemel


Music:
----------
1. Chopin - Nocturne in E Flat Major
   - https://www.youtube.com/watch?v=p29JUpsOSTE


Technology Used:
-------------------------
    Rendering Technology    - OpenGL
    HPP Technology               - CUDA
    Windowing                       - Win32
    OS                                       - Windows 10
    Language                           - C++


Hashtags:
---------------
    #AstroMediComp
    #RealTimeRendering
    #DomainGroup
    #OpenGL
    #CUDA
    #Win32
    #C++
    #Windows
    #RTR2020

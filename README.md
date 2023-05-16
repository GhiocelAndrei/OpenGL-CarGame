# OpenGL Car Game

## Description
Simple Car Game created using OpenGL library. I have generated from scratch : track, car, trees and background. Also, I added self driving obstacles that follow a certain path.
Colliding with obstacles will stop the car movements for 3 seconds.

## Features 

#### Self-generated track :
 I generated the track using an array of coordinates. Using those coordinates I generated two different  set of points one for the exterior track and one for the interior track, using the formula :
 Let's assume we have 2 consecutive points P1(x1, 0, z1) and P2(x2, 0, z2) from a set of points that define the path:

 1.We determine the direction vector from P1 to P2, denoted as D (black in the picture).
 2.We determine the vector perpendicular to D, denoted as P (green in the picture): P = cross(D, UP), where UP is the vector perpendicular to the XOZ plane.
 3.Using the vector P and the position of P1, we can obtain the exterior/interior points (Red and Blue): R = P1 + distRed * P, A = P1 - distBlue * P.
 
![egc_poza1](https://github.com/GhiocelAndrei/OpenGL-CarGame/assets/105803228/c23d934e-a014-487e-b6e6-42bb80495abc)

 Therefore, I applied the same process to all the segments, obtaining the new sets of exterior/interior points:
 
  ![egc_poza2](https://github.com/GhiocelAndrei/OpenGL-CarGame/assets/105803228/378d5117-ebfb-4231-850a-c29cb131152b)

  Having the two sets (exterior and interior), we can construct the surface of the track. Let's assume we have the exterior points r1, r2, r3, and the interior points a1, a2, a3. We need to construct the four triangles: (r1, a1, a2), (r1, a2, r2), (r2, a2, a3), and (r2, a3, r3).
 
 ![egc_poza3](https://github.com/GhiocelAndrei/OpenGL-CarGame/assets/105803228/1a8eb28f-43e3-4e79-8a28-39467d0a9a00)

  By applying the same process to all points in both sets, I obtained the track.
 
#### Self-generated environment and car :
 I also created the environment : cars, floor, sky and car from scratch.
 
#### Car control :
 Car control is created by me using transformation and rotation. I also created a camera that I places behind the car. There is also a mechanism that checks if the car is on the track, stopping the car from leaving said track.
 
#### Design :
 I also added a minimap for tracking the car from above and shaders.

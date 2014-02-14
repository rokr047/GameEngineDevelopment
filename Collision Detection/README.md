Collision Detection
=====================
http://www.youtube.com/watch?v=PWoj8smO00k

This video is a demonstration of the simple collision system my team built into the game engine course work assignment. All the data is fed into game engine from Maya.
The scene has the following :
1. Gravity. we use sphere box collision detection for the gravity, the bottom sphere collider (orange box) keeps colliding with the plane (box collider), if not, then you see the soldier falling. [You dont see 1 soldier falling because the isCollidable is set to false, which means he has no physics attributes.]
2. Soldier - Soldier collision, we use the top sphere (blue box) to check for collision between 2 soldier.
3. Soldier - Mesh (car/enemy) we use Sphere - Box collider for soldier and box collider for immovable objects in the scene. You can see the soldier sliding on the surface of the car.

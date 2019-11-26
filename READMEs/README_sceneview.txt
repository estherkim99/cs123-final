Sceneview Project

- Design decision:
I stored the information needed to render primitives (primitive type, transformation matrix, and material properties) in a std::vector to traverse through the scene nodes more efficiently.
Using a recursive function, parseNode in Scene class, I traversed through the scene tree until there are no children node.

- Any known bugs:
Camera transformation is really slow compared to TA.

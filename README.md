# CG Final Projects 2022

development logs:

- main.cpp

- shader.h/shader.cpp

  重构shader

- skybox.h/skybox.cpp  //faces1,2

  加载天空盒

- mesh.h/mesh.cpp

  https://learnopengl-cn.github.io/03%20Model%20Loading/02%20Mesh/

  通过使用Assimp，我们可以加载不同的模型到程序中，但是载入后它们都被储存为Assimp的数据结构。我们最终仍要将这些数据转换为OpenGL能够理解的格式，这样才能渲染这个物体。

  一个网格应该至少需要一系列的顶点，每个顶点包含一个位置向量、一个法向量和一个纹理坐标向量。一个网格还应该包含用于索引绘制的索引以及纹理形式的材质数据（漫反射/镜面光贴图）。

- model.h/model.cpp

  https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/

- plymodel.h/plymodel.cpp

  http://graphics.stanford.edu/data/3Dscanrep/

- texture.h/texture.cpp

- camera.h/camera.cpp

- diymodel.h/diymodel.cpp

- 

know pbr photo



T- Turn on the light

WASD- move

已经实现：

1. 基本几何变换
2. 光源编辑
3. 基本体素建模
4. 能够漫游

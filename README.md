# DFLighting
Dwarf Fortress Lighting project that was created during hurricane Sandy just for fun, the cold and dark atmosphere was right for such a project. This Lighting Engine was designed to handle thousands of 3D point lights in real-time in a boxed world. 

I'm refactoring it (prototype was very dirty: no optimizations, no memory disposal, no smart pointers, no AVX instrinsics, slow renderer and naive A*).


ALL LIGHTS ARE true 3D LIGHTS, not 2D!!!!!!!!!!!!!!!!


Check this out (more on my youtube channel). Note that all slowdowns are because of very naive A* algorithm.


First demo
<a href="http://www.youtube.com/watch?feature=player_embedded&v=wih3k0wnsnc" target="_blank"><img src="http://img.youtube.com/vi/wih3k0wnsnc/0.jpg" alt="First demo" width="854" height="510" border="10" /></a>


Colors. Memorizing a fortress.
<a href="http://www.youtube.com/watch?feature=player_embedded&v=6MmgC6TMkjw" target="_blank"><img src="http://img.youtube.com/vi/6MmgC6TMkjw/0.jpg" alt="Colors. Memorizing a fortress." width="854" height="510" border="10" /></a>


Tree shadows.
<a href="http://www.youtube.com/watch?feature=player_embedded&v=-ncqex8ijIM" target="_blank"><img src="http://img.youtube.com/vi/-ncqex8ijIM/0.jpg" alt="Outside. Trees." width="854" height="510" border="10" /></a>

-- a3mark.lua
-- A very simple scene creating a trivial hierarchical puppet.
-- We'll use this scene as part of testing your assignment.
-- See a3mark.png for an example of what this file produces.

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
dark_green = gr.material({0.0, 0.5, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
purple = gr.material({0.7, 0.3, 0.85}, {0.1, 0.1, 0.1}, 10)

block = gr.mesh( 'cube', 'tx1_block' )
rootnode:add_child( block )
block:translate( 0.0, 0.5, 0 )
block:scale( 15.0, 7.0, 6.0 )
--block:scale( 15.0, 20.0, 6.0 )
block:set_material( red )

--rootnode:scale( 50.0, 1.0, 50.0 )
--rootnode:translate(0.0, -0.5, 0.0)

return rootnode

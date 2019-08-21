-- a3mark.lua
-- A very simple scene creating a trivial hierarchical puppet.
-- We'll use this scene as part of testing your assignment.
-- See a3mark.png for an example of what this file produces.

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({1.0, 0.8, 0.8}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
dark_green = gr.material({0.0, 0.5, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
purple = gr.material({0.7, 0.3, 0.85}, {0.1, 0.1, 0.1}, 10)

orb = gr.mesh( 'sphere', 'orb' )
rootnode:add_child( orb )
orb:translate( 0.0, 2.0, 0 )
orb:scale( 4.0, 4.0, 4.0 )
orb:set_material( blue )

--rootnode:scale( 50.0, 1.0, 50.0 )
--rootnode:translate(0.0, -0.5, 0.0)

return rootnode

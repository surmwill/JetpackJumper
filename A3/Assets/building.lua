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
grey = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 10)

building = gr.mesh( 'cube', 'tx0_building' )
rootnode:add_child( building )
building:translate( 0.0, -0.5, 0.0 )
building:scale( 50.0, 1500.0, 50.0 )
building:translate( 0.0, -1.0, 0.0 )
building:set_material( red )

building_top = gr.mesh( 'cube', 'ntx_top_building' )
rootnode:add_child( building_top )
building_top:translate( 0.0, -0.5, 0.0 )
building_top:scale( 50.0, 1.0, 50.0 )
building_top:set_material( grey )


--building2 = gr.mesh( 'cube', 'building2' )
--rootnode:add_child( building2 )
--building2:translate( 1.0, 0.5, 0.0 )
--building2:scale( 6.0, 6.0, 6.0 )
--building2:set_material( blue )

--building3 = gr.mesh( 'sphere', 'building3' )
--rootnode:add_child( building3 )
--building3:translate( 3.0, 1.0, 0 )
--building3:scale( 4.0, 4.0, 4.0 )
--building3:set_material( red )

--rootnode:scale( 50.0, 1.0, 50.0 )
--rootnode:translate(0.0, -0.5, 0.0)

return rootnode

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

-- TORSO JOINT --
torso_joint = gr.joint( 'torso_joint', { -35.0, 0.0, 35.0 }, { 0.0, 0.0, 0.0 } )
rootnode:add_child( torso_joint )
torso_joint:translate( 0.0, -2.0, 0.0 ) 
-- TORSO --
torso = gr.mesh( 'cube', 'torso' )
torso_joint:add_child( torso )
torso:set_material( blue )
torso:scale( 2.0, 4.0, 1.25 )
-- UPPER TORSO
upper_torso = gr.mesh( 'cube', 'upper_torso' )
torso:add_child( upper_torso )
upper_torso:set_material( dark_green ) 
upper_torso:translate( 0.0, 0.5, 0.0 )
upper_torso:scale( 2.0, 1.0, 1.25 )
upper_torso:translate( 0.0, 2.0, 0.0 ) 

-- LOWER NECK JOINT --
lower_neck_joint = gr.joint( 'lower_neck_joint', { -35.0, 0.0, 35.0 }, { 0.0, 0.0, 0.0 } )
torso:add_child( lower_neck_joint )
lower_neck_joint:translate( 0.0, 2.5, 0.0 ) 
-- NECK --
neck = gr.mesh( 'cube', 'neck' )
lower_neck_joint:add_child( neck )
neck:set_material( blue )
neck:translate( 0.0, 0.5, 0.0 )
neck:scale( 0.6, 0.75, 0.6 )
neck:translate( 0.0, 2.8, 0.0 )
-- UPPER NECK JOINT -- 
upper_neck_joint = gr.joint( 'upper_neck_joint', { 0.0, 0.0, 0.0 }, { -90.0, 0.0, 90.0 } )
neck:add_child( upper_neck_joint )
upper_neck_joint:translate( 0.0, 4.75, 0.0 )
-- HEAD --
head = gr.mesh( 'sphere', 'head' )
upper_neck_joint:add_child( head )
head:set_material( red )
head:translate( 0.0, 1.0, 0.0 )
head:scale( 0.85, 0.85, 0.85 )
head:translate( 0.0, 3.25, 0.0 )
-- LEFT EYE  --
l_eye = gr.mesh( 'cube', 'l_eye' )
head:add_child( l_eye )
l_eye:set_material( green )
l_eye:scale( 0.3, 0.3, 0.3 )
l_eye:translate( -0.2, 4.2, 0.8 )
-- RIGHT EYE  --
r_eye = gr.mesh( 'cube', 'r_eye' )
head:add_child( r_eye )
r_eye:set_material( green )
r_eye:scale( 0.3, 0.3, 0.3 )
r_eye:translate( 0.2, 4.2, 0.8 )

-- LEFT SHOULDER --
l_shoulder = gr.mesh( 'cube', 'l_shoulder' )
upper_torso:add_child( l_shoulder )
l_shoulder:set_material( green )
l_shoulder:scale( 1.0, 1.0, 1.25 )
l_shoulder:translate( -1.5, 2.5, 0.0 )
-- LEFT SHOULDER JOINT --
l_shoulder_joint = gr.joint( 'l_shoulder_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
torso:add_child( l_shoulder_joint )
l_shoulder_joint:translate( -1.0, 2.5, 0.0 )
-- LEFT UPPER ARM -- 
l_upper_arm = gr.mesh( 'cube', 'l_upper_arm' )
l_shoulder_joint:add_child( l_upper_arm )
l_upper_arm:set_material( red )
l_upper_arm:scale( 0.5, 2.0, 0.5 )
l_upper_arm:translate( -1.5, 1.5, 0.0 )
-- LEFT ELBOW --
l_elbow = gr.mesh( 'cube', 'l_elbow' )
l_upper_arm:add_child( l_elbow )
l_elbow:set_material( green )
l_elbow:scale( 0.75, 0.75, 0.75 )
l_elbow:translate( -1.5, 0.5, 0.0 )
-- LEFT ELBOW JOINT -- 
l_elbow_joint = gr.joint( 'l_elbow_joint', { -90.0, 0.0, 30.0 }, { 0.0, 0.0, 0.0 } )
l_elbow:add_child( l_elbow_joint )
l_elbow_joint:translate( -1.5, 0.5, 0.0 ) -- 1.75 for y
-- LEFT LOWER ARM -- 
l_lower_arm = gr.mesh( 'cube', 'l_lower_arm' )
l_elbow_joint:add_child( l_lower_arm )
l_lower_arm:set_material( blue )
l_lower_arm:scale( 0.5, 2.0, 0.5 )
l_lower_arm:translate( -1.5, -0.5, 0.0 ) -- 1.75 for y
-- LEFT WRIST --
l_wrist = gr.mesh( 'cube', 'l_wrist' )
l_lower_arm:add_child( l_wrist )
l_wrist:set_material( green )
l_wrist:scale( 0.75, 0.5, 0.75 )
l_wrist:translate( -1.5, -1.5, 0.0 ) 
-- LEFT WRIST JOINT -- 
l_wrist_joint = gr.joint( 'l_wrist_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
l_wrist:add_child( l_wrist_joint )
l_wrist_joint:translate( -1.5, -1.55, 0.0 ) -- 1.75 for y
-- LEFT HAND --
l_hand = gr.mesh( 'cube', 'l_hand' )
l_wrist_joint:add_child( l_hand )
l_hand:set_material( red )
l_hand:scale( 0.5, 0.8, 0.3 )
l_hand:translate( -1.5, -1.75, 0.0 ) 

-- RIGHT SHOULDER --
r_shoulder = gr.mesh( 'cube', 'r_shoulder' )
upper_torso:add_child( r_shoulder )
r_shoulder:set_material( green )
r_shoulder:scale( 1.0, 1.0, 1.25 )
r_shoulder:translate( 1.5, 2.5, 0.0 )
-- RIGHT SHOULDER JOINT --
r_shoulder_joint = gr.joint( 'r_shoulder_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
torso:add_child( r_shoulder_joint )
r_shoulder_joint:translate( 1.0, 2.5, 0.0 )
-- RIGHT UPPER ARM -- 
r_upper_arm = gr.mesh( 'cube', 'r_upper_arm' )
r_shoulder_joint:add_child( r_upper_arm )
r_upper_arm:set_material( red )
r_upper_arm:scale( 0.5, 2.0, 0.5 )
r_upper_arm:translate( 1.5, 1.5, 0.0 )
-- RIGHT ELBOW --
r_elbow = gr.mesh( 'cube', 'r_elbow' )
r_upper_arm:add_child( r_elbow )
r_elbow:set_material( green )
r_elbow:scale( 0.75, 0.75, 0.75 )
r_elbow:translate( 1.5, 0.5, 0.0 )
-- RIGHT ELBOW JOINT -- 
r_elbow_joint = gr.joint( 'r_elbow_joint', { -90.0, 0.0, 30.0 }, { 0.0, 0.0, 0.0 } )
r_elbow:add_child( r_elbow_joint )
r_elbow_joint:translate( 1.5, 0.5, 0.0 ) -- 1.75 for y
-- RIGHT LOWER ARM -- 
r_lower_arm = gr.mesh( 'cube', 'r_lower_arm' )
r_elbow_joint:add_child( r_lower_arm )
r_lower_arm:set_material( blue )
r_lower_arm:scale( 0.5, 2.0, 0.5 )
r_lower_arm:translate( 1.5, -0.5, 0.0 ) -- 1.75 for y
-- RIGHT WRIST --
r_wrist = gr.mesh( 'cube', 'r_wrist' )
r_lower_arm:add_child( r_wrist )
r_wrist:set_material( green )
r_wrist:scale( 0.75, 0.5, 0.75 )
r_wrist:translate( 1.5, -1.5, 0.0 ) 
-- RIGHT WRIST JOINT -- 
r_wrist_joint = gr.joint( 'r_wrist_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
r_wrist:add_child( r_wrist_joint )
r_wrist_joint:translate( 1.5, -1.55, 0.0 ) -- 1.75 for y
-- RIGHT HAND --
r_hand = gr.mesh( 'cube', 'r_hand' )
r_wrist_joint:add_child( r_hand )
r_hand:set_material( red )
r_hand:scale( 0.5, 0.8, 0.3 )
r_hand:translate( 1.5, -1.75, 0.0 ) 

-- LEFT UPPER LEG JOINT --
l_upper_leg_joint = gr.joint( 'l_upper_leg_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
rootnode:add_child( l_upper_leg_joint )
l_upper_leg_joint:translate( -0.5, -2.0, 0.0 )
-- LEFT UPPER LEG -- 
l_upper_leg = gr.mesh( 'cube', 'l_upper_leg' )
l_upper_leg_joint:add_child( l_upper_leg )
l_upper_leg:set_material( red )
l_upper_leg:scale( 0.6, 2.1, 0.6 )
l_upper_leg:translate( -0.5, -2.5, 0.0 )
-- LEFT KNEE --
l_knee = gr.mesh( 'cube', 'l_knee' )
l_upper_leg:add_child( l_knee )
l_knee:set_material( green )
l_knee:scale( 0.8, 0.75, 0.8 )
l_knee:translate( -0.5, -3.7, 0.0 )
-- LEFT KNEE JOINT -- 
l_knee_joint = gr.joint( 'l_knee_joint', { -30.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
l_knee:add_child( l_knee_joint )
l_knee_joint:translate( -0.5, -3.7, 0.0 ) -- 1.75 for y
-- LEFT LOWER LEG -- 
l_lower_leg = gr.mesh( 'cube', 'l_lower_leg' )
l_knee_joint:add_child( l_lower_leg )
l_lower_leg:set_material( blue )
l_lower_leg:scale( 0.5, 2.1, 0.5 )
l_lower_leg:translate( -0.5, -4.5, 0.0 ) -- 1.75 for y
-- LEFT ANKLE JOINT -- 
l_ankle_joint = gr.joint( 'l_ankle_joint', { 0.0, 0.0, 0.0 }, { -45.0, 0.0, 45.0 } )
l_lower_leg:add_child( l_ankle_joint )
l_ankle_joint:translate( -0.5, -5.7, 0.0 ) -- 1.75 for y
-- LEFT FOOT --
l_foot = gr.mesh( 'cube', 'l_foot' )
l_ankle_joint:add_child( l_foot )
l_foot:set_material( red )
l_foot:scale( 0.8, 0.4, 1.5 )
l_foot:translate( -0.5, -5.5, 0.3 ) 

-- RIGHT UPPER LEG JOINT --
r_upper_leg_joint = gr.joint( 'r_upper_leg_joint', { -90.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
rootnode:add_child( r_upper_leg_joint )
r_upper_leg_joint:translate( 0.5, -2.0, 0.0 )
-- RIGHT UPPER LEG -- 
r_upper_leg = gr.mesh( 'cube', 'r_upper_leg' )
r_upper_leg_joint:add_child( r_upper_leg )
r_upper_leg:set_material( red )
r_upper_leg:scale( 0.6, 2.1, 0.6 )
r_upper_leg:translate( 0.5, -2.5, 0.0 )
-- RIGHT KNEE --
r_knee = gr.mesh( 'cube', 'r_knee' )
r_upper_leg:add_child( r_knee )
r_knee:set_material( green )
r_knee:scale( 0.8, 0.75, 0.8 )
r_knee:translate( 0.5, -3.7, 0.0 )
-- RIGHT KNEE JOINT -- 
r_knee_joint = gr.joint( 'r_knee_joint', { -30.0, 0.0, 90.0 }, { 0.0, 0.0, 0.0 } )
r_knee:add_child( r_knee_joint )
r_knee_joint:translate( 0.5, -3.7, 0.0 ) -- 1.75 for y
-- RIGHT LOWER LEG -- 
r_lower_leg = gr.mesh( 'cube', 'r_lower_leg' )
r_knee_joint:add_child( r_lower_leg )
r_lower_leg:set_material( blue )
r_lower_leg:scale( 0.5, 2.1, 0.5 )
r_lower_leg:translate( 0.5, -4.5, 0.0 ) -- 1.75 for y
-- RIGHT ANKLE JOINT -- 
r_ankle_joint = gr.joint( 'r_ankle_joint', { 0.0, 0.0, 0.0 }, { -45.0, 0.0, 45.0 } )
r_lower_leg:add_child( r_ankle_joint )
r_ankle_joint:translate( 0.5, -5.7, 0.0 ) -- 1.75 for y
-- RIGHT FOOT --
r_foot = gr.mesh( 'cube', 'r_foot' )
r_ankle_joint:add_child( r_foot )
r_foot:set_material( red )
r_foot:scale( 0.8, 0.4, 1.5 )
r_foot:translate( 0.5, -5.5, 0.3 ) 

rootnode:translate(0.0, 0.0, -50.0)

return rootnode

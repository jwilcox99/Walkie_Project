// CXS Walkie MK1.1 - Heltec Mount
include <cxs_params.scad>

module heltec_clip_standoffs() {
    // MK1.1 Heltec mount: side-wall bridge + true cantilever spring clips.
    // Board is landscape: X = heltec_l, Y = heltec_w.
    // Keep final mounting height here until USB opening is added.

    mount_z = wall + heltec_standoff_h;

    board_x = heltec_l;
    board_y = heltec_w;

    rail_h = 3.0;
    rail_w = 3.0;
    rail_clearance = 0.8;

    wall_h = 7.5;
    tray_wall_t = 2.2;

    clip_arm_w = 4.0;          // X width of each spring finger
    clip_arm_l = 8.5;          // Y spring length
    clip_arm_t = 1.2;          // Z thickness/flex section
    clip_lip_h = 1.4;
    clip_lip_overhang = 1.1;
    clip_gap = 0.6;

    fixed_y = heltec_y - board_y/2 - rail_clearance;
    clip_y  = heltec_y + board_y/2 + rail_clearance;

    wall_z = mount_z + rail_h/2 + wall_h/2;
    bridge_x = case_w - 2*wall + 1;

    // Full-width bridge rails physically tie into the case side walls.
    translate([0, fixed_y, mount_z])
        cube([bridge_x, rail_w, rail_h], center=true);

    translate([0, clip_y, mount_z])
        cube([bridge_x, rail_w, rail_h], center=true);

    // Fixed lower locating wall. This side is rigid.
    translate([0, fixed_y - 1.2, wall_z])
        cube([bridge_x, tray_wall_t, wall_h], center=true);

    // Clip-side low wall sections. Leave gaps where spring clips live.
    for (xseg = [-1, 1]) {
        translate([xseg * bridge_x/4, clip_y + 1.2, wall_z])
            cube([bridge_x/2 - 12, tray_wall_t, wall_h], center=true);
    }

    // PCB support ledges on top of the bridge rails.
    translate([heltec_x, fixed_y, mount_z + rail_h])
        cube([board_x, rail_w + 1, 1.2], center=true);

    translate([heltec_x, clip_y, mount_z + rail_h])
        cube([board_x, rail_w + 1, 1.2], center=true);

    // End stops around the board area only.
    for (xoff = [-board_x/2, board_x/2])
        translate([heltec_x + xoff, heltec_y, wall_z])
            cube([tray_wall_t, board_y + 6, wall_h], center=true);


    // True cantilever spring clips on the opposite rail.
    // Each finger is anchored into the side-wall bridge and bends outward during insertion.
    for (xoff = [-board_x/4, board_x/4]) {
        // flexible arm
        translate([heltec_x + xoff, clip_y + clip_arm_l/2, mount_z + rail_h + heltec_h + clip_gap])
            cube([clip_arm_w, clip_arm_l, clip_arm_t], center=true);

        // root block merges into the bridge / side-wall lip, avoiding floating geometry
        translate([heltec_x + xoff, clip_y + 1.8, mount_z + rail_h + heltec_h/2])
            cube([clip_arm_w + 1.2, 3.2, heltec_h + rail_h], center=true);

        // inward lip that captures the PCB edge
        translate([heltec_x + xoff, clip_y - clip_lip_overhang/2, mount_z + rail_h + heltec_h + clip_lip_h/2 + clip_gap])
            cube([clip_arm_w + 0.8, clip_lip_overhang, clip_lip_h], center=true);
    }
}

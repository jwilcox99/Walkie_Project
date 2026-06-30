// CXS Walkie MK1 - Heltec Mount
include <cxs_params.scad>

module heltec_clip_standoffs() {
    // MK1 Heltec mount: side-wall supported tray + snap clips
    // Board is landscape: X = heltec_l, Y = heltec_w

    mount_z = wall + heltec_standoff_h;

    rail_h = 3.0;
    rail_w = 3.0;
    rail_clearance = 0.8;

    wall_h = 8;
    tray_wall_t = 2.2;

    clip_w = 5.0;
    clip_l = 8.0;
    lip_h = 1.4;
    lip_overhang = 1.2;

    board_x = heltec_l;
    board_y = heltec_w;

    fixed_y = heltec_y - board_y/2 - rail_clearance;
    clip_y  = heltec_y + board_y/2 + rail_clearance;

    wall_z = mount_z + rail_h/2 + wall_h/2;

    // Full-width bridge rails that tie into the case side walls.
    // These are longer than the board so they physically merge with the shell.
    bridge_x = case_w - 2*wall + 1;

    translate([0, fixed_y, mount_z])
        cube([bridge_x, rail_w, rail_h], center=true);

    translate([0, clip_y, mount_z])
        cube([bridge_x, rail_w, rail_h], center=true);

    // Long vertical locating walls on the rails.
    translate([0, fixed_y - 1.2, wall_z])
        cube([bridge_x, tray_wall_t, wall_h], center=true);

    translate([0, clip_y + 1.2, wall_z])
        cube([bridge_x, tray_wall_t, wall_h], center=true);

    // Board-length ledges on top of the bridge rails.
    // These define the actual PCB support area.
    translate([heltec_x, fixed_y, mount_z + rail_h])
        cube([board_x, rail_w + 1, 1.2], center=true);

    translate([heltec_x, clip_y, mount_z + rail_h])
        cube([board_x, rail_w + 1, 1.2], center=true);

    // End stops only around board area, not full case width.
    for (xoff = [-board_x/2, board_x/2])
        translate([heltec_x + xoff, heltec_y, wall_z])
            cube([tray_wall_t, board_y + 6, wall_h], center=true);
}
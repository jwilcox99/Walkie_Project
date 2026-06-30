// CXS Walkie MK1 - Battery Holder
include <cxs_params.scad>

module battery_clip_holder() {
    // Curved side-clip 18650 holder for inside back shell.
    // Uses existing params: battery_x, battery_y, battery_d, battery_l, wall

    clip_wall = 2.2;
    clip_width = 7.0;      // length along battery
    clip_outer_d = battery_d + 2 * clip_wall;
    clip_positions = [-24, 0, 24];

    saddle_w = battery_d + 3.0;
    saddle_t = 2.2;
    saddle_h = 4.0;

    // Low saddle ribs under battery
    for (yoff = [-30, -10, 10, 30]) {
        translate([battery_x, battery_y + yoff, wall + 2])
            difference() {
                cube([saddle_w, saddle_t, saddle_h], center=true);

                translate([0, 0, battery_d/2 + 0.5])
                    rotate([90,0,0])
                        cylinder(d=battery_d, h=saddle_t + 0.4, center=true);
            }
    }

    // Curved side clips, open at top
    // Lowered so the clip feet connect to the shell floor.
    clip_z_offset = -5;

    for (yoff = clip_positions) {
        translate([battery_x, battery_y + yoff, wall + battery_d/2 + clip_z_offset])
            rotate([90,0,0])
                difference() {
                    cylinder(d=clip_outer_d, h=clip_width, center=true);
                    cylinder(d=battery_d + 0.8, h=clip_width + 0.4, center=true);

                    // remove lower part, but leave enough material to touch the floor
                    translate([0, -clip_outer_d/2 - 3, 0])
                        cube([clip_outer_d + 2, clip_outer_d, clip_width + 1], center=true);

                    // remove wide top opening so battery snaps in
                    translate([0, clip_outer_d/2, 0])
                        cube([clip_outer_d * 0.75, clip_outer_d, clip_width + 1], center=true);
                }
    }

    // No bottom end stop needed.
    // The battery sits against the bottom of the case.

    // Short top end stops, split left/right so they avoid the channel button area
    top_stop_y = battery_y + battery_l/2 + 2;

    for (xoff = [-7, 7])
        translate([battery_x + xoff, top_stop_y, wall + 3])
            cube([7, 4, 3], center=true);

    // Taller center keeper only in the middle
    // Keeps the battery from sliding upward without interfering with side controls
    translate([battery_x, top_stop_y, wall + 5])
        cube([8, 4, 10], center=true);
}

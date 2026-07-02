// CXS Walkie MK1.1 - Battery Holder
include <cxs_params.scad>

module battery_clip_holder() {
    // MK1.1 18650 holder: printable saddles + straight spring side clips.
    // Battery moved toward center using battery_x in cxs_params.scad.

    saddle_w = battery_d + 3.0;
    saddle_t = 2.2;
    saddle_h = 4.0;

    clip_positions = [-24, 0, 24];
    tab_w = 2.0;
    tab_l = 8.0;
    tab_h = 12.0;
    tab_inset = 1.0;
    lead_in_h = 3.0;

    // Low curved saddle ribs under battery.
    for (yoff = [-30, -10, 10, 30]) {
        translate([battery_x, battery_y + yoff, wall + 2])
            difference() {
                cube([saddle_w, saddle_t, saddle_h], center=true);

                translate([0, 0, battery_d/2 + 0.5])
                    rotate([90,0,0])
                        cylinder(d=battery_d, h=saddle_t + 0.4, center=true);
            }
    }

    // Printable side spring tabs. These avoid the unsupported curved clip problem.
    for (yoff = clip_positions)
        for (side = [-1, 1]) {
            x = battery_x + side * (battery_d/2 + tab_w/2 - tab_inset);

            // main spring tab rising from shell floor
            translate([x, battery_y + yoff, wall + tab_h/2])
                cube([tab_w, tab_l, tab_h], center=true);

            // small inward lead-in/capture nub near the top
            translate([battery_x + side * (battery_d/2 - 0.6), battery_y + yoff, wall + tab_h - lead_in_h/2])
                rotate([0, side * -18, 0])
                    cube([tab_w + 0.8, tab_l, lead_in_h], center=true);
        }

    // Top end stop lowered to match the top support rib height.
    top_stop_y = battery_y + battery_l/2 + 2;

    for (xoff = [-7, 7])
        translate([battery_x + xoff, top_stop_y, wall + 3])
            cube([7, 4, 3], center=true);

    translate([battery_x, top_stop_y, wall + 3.5])
        cube([8, 4, 4], center=true);
}

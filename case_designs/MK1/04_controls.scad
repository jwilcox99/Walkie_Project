// CXS Walkie MK1 - Controls and Side Cutouts
include <cxs_params.scad>

module ptt_cutout() {
    translate([left_side_x, ptt_y, back_shell_d/2])
        rotate([0,90,0])
            cylinder(d = ptt_hole_d, h = hole_depth, center=true);
}

module channel_cutout() {
    translate([right_side_x, channel_y, back_shell_d/2])
        rotate([0,90,0])
            cylinder(d = channel_hole_d, h = hole_depth, center=true);
}

module power_cutout() {
    translate([right_side_x, power_y, back_shell_d/2])
        rotate([0,90,0])
            cylinder(d = power_hole_d, h = hole_depth, center=true);
}

module side_cutouts() {
    ptt_cutout();
    channel_cutout();
    power_cutout();

    // USB-C side cutout intentionally removed for MK1 cleanup.
    // Antenna hole intentionally removed until antenna routing is finalized.
}

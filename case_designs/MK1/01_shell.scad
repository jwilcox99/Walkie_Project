// CXS Walkie MK1 - Shell
include <cxs_params.scad>

$fn = 48;

module rounded_box(w, h, d, r) {
    hull() {
        for (x = [-w/2 + r, w/2 - r])
            for (y = [r, h - r])
                translate([x, y, 0])
                    cylinder(r = r, h = d);
    }
}

module shell_body(depth) {
    difference() {
        rounded_box(case_w, case_h, depth, corner_r);

        // Inner cavity shifted up by wall thickness to preserve bottom wall.
        translate([0, wall, wall])
            rounded_box(
                case_w - 2*wall,
                case_h - 2*wall,
                depth + 0.2,
                max(1, corner_r - wall)
            );
    }
}

module screw_bosses(depth, clearance=false) {
    for (x = [-case_w/2 + corner_screw_margin_x, case_w/2 - corner_screw_margin_x])
        for (y = [corner_screw_margin_y, case_h - corner_screw_margin_y])
            difference() {
                translate([x, y, wall])
                    cylinder(d = clearance ? 7.5 : 8.5, h = depth - wall);
                translate([x, y, -0.1])
                    cylinder(d = clearance ? screw_clearance_d : insert_d, h = depth + 0.3);
            }
}

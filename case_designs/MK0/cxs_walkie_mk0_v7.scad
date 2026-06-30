// CXS Walkie Mk0 v7
// Centered Heltec/battery layout, lower amp, multi-hole mic pattern.
// Uses the shared parameter file.
// Keep cxs_walkie_params_v2.scad in the same folder as this file.

include <cxs_walkie_params_v5.scad>

$fn = 48;

preview_gap = 28;

// ---------- Helpers ----------
module rounded_box(w, h, d, r) {
    hull() {
        for (x = [-w/2 + r, w/2 - r])
            for (y = [r, h - r])
                translate([x, y, 0])
                    cylinder(r = r, h = d);
    }
}

module mic_standoffs() {
    for (xoff = [-mic_mount_hole_spacing/2, mic_mount_hole_spacing/2]) {
        difference() {
            translate([mic_x + xoff, mic_y + mic_mount_y_offset, wall])
                cylinder(d = mic_standoff_d, h = mic_standoff_h);

            translate([mic_x + xoff, mic_y + mic_mount_y_offset, wall - 0.1])
                cylinder(d = mic_screw_d, h = mic_standoff_h + 0.3);
        }
    }
}

module shell_body(depth) {
    difference() {
        rounded_box(case_w, case_h, depth, corner_r);

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

// ---------- Front Face ----------
module speaker_grille_cutouts() {
    total_w = speaker_grille_slots * speaker_grille_slot_w
            + (speaker_grille_slots - 1) * speaker_grille_slot_gap;

    for (i = [0:speaker_grille_slots-1]) {
        x = speaker_x - total_w/2 + i*(speaker_grille_slot_w + speaker_grille_slot_gap);
        translate([x, speaker_y, -1])
            cube([speaker_grille_slot_w, speaker_grille_slot_h, front_shell_d + 2], center=true);
    }
}


module mic_hole_pattern() {
    // Three small clusters: center plus left/right. Better voice pickup and still printable.
    for (cx = [mic_x - mic_hole_cluster_spacing, mic_x, mic_x + mic_hole_cluster_spacing]) {
        translate([cx, mic_y, -1])
            cylinder(d = mic_hole_d, h = front_shell_d + 2);

        translate([cx - mic_hole_pattern_spread/2, mic_y - mic_hole_pattern_spread/2, -1])
            cylinder(d = mic_hole_d, h = front_shell_d + 2);

        translate([cx + mic_hole_pattern_spread/2, mic_y - mic_hole_pattern_spread/2, -1])
            cylinder(d = mic_hole_d, h = front_shell_d + 2);

        translate([cx - mic_hole_pattern_spread/2, mic_y + mic_hole_pattern_spread/2, -1])
            cylinder(d = mic_hole_d, h = front_shell_d + 2);

        translate([cx + mic_hole_pattern_spread/2, mic_y + mic_hole_pattern_spread/2, -1])
            cylinder(d = mic_hole_d, h = front_shell_d + 2);
    }
}

module heltec_clip_standoffs() {
    // Board mounted landscape:
    // X dimension = heltec_l
    // Y dimension = heltec_w

    post_x = heltec_l/2 - 5;
    post_y = heltec_w/2 + 2;

    // Four raised posts near board corners
    for (xoff = [-post_x, post_x])
        for (yoff = [-post_y, post_y])
            translate([heltec_x + xoff, heltec_y + yoff, wall])
                cylinder(d = heltec_post_d, h = heltec_standoff_h);

    // Bottom support pads under the board
    for (xoff = [-post_x, post_x])
        for (yoff = [-post_y, post_y])
            translate([heltec_x + xoff, heltec_y + yoff, wall + heltec_standoff_h])
                cube([7, 5, 1.2], center=true);

    // Side retaining clips along top/bottom edges
    for (yoff = [-heltec_w/2 - heltec_clip_clearance,
                  heltec_w/2 + heltec_clip_clearance]) {
        translate([heltec_x, heltec_y + yoff, wall + heltec_standoff_h + heltec_h/2])
            cube([heltec_l - 8, heltec_clip_w, heltec_h], center=true);

        // Small inward lip to catch board edge
        translate([heltec_x, heltec_y + yoff, wall + heltec_standoff_h + heltec_h + heltec_clip_lip_h/2])
            cube([heltec_l - 8, heltec_clip_lip_overhang, heltec_clip_lip_h], center=true);
    }
}

module front_shell() {
    difference() {
        union() {
            shell_body(front_shell_d);
            screw_bosses(front_shell_d, true);
            mic_standoffs();
        }

        // OLED window only on the front face.
        translate([oled_x, oled_y, -1])
            cube([oled_window_w, oled_window_h, front_shell_d + 2], center=true);

        // Speaker grille only on the front face.
        speaker_grille_cutouts();

        // Centered mic grille holes
        for (x = [-mic_hole_spacing_x, 0, mic_hole_spacing_x])
            for (y = [-mic_hole_spacing_y/2, mic_hole_spacing_y/2])
                translate([mic_x + x, mic_y + y, -1])
                    cylinder(d = mic_hole_d, h = front_shell_d + 2);
    }
}

// ---------- Back Tub ----------
module side_button_cutouts() {
    // PTT on left side. Hole through side wall only.
    translate([left_side_x, ptt_y, back_shell_d/2])
    rotate([0,90,0])
        cylinder(d = ptt_hole_d, h = hole_depth, center=true);
    
    // Channel on right side.
translate([right_side_x, ptt_y + 10, back_shell_d/2])
    rotate([0,90,0])
        cylinder(d = channel_hole_d, h = hole_depth, center=true);

// Power toggle on right side, below channel.
translate([right_side_x, ptt_y - 16, back_shell_d/2])
    rotate([0,90,0])
        cylinder(d = power_hole_d, h = hole_depth, center=true);

// USB-C side cutout near Heltec, higher on body, cuts fully through side wall
usb_y = heltec_y + 2;   // increase/decrease this to move it up/down
usb_z = back_shell_d / 2 + 6;

translate([right_side_x, usb_y, usb_z])
    cube([24, usb_c_w + 4, usb_c_h + 5], center=true);
}

module battery_clip_holder() {
    // Vertical 18650 holder using rails/clips instead of a full tube.
    rail_h = battery_l;
    rail_w = 3;
    rail_d = 8;

    // Side rails.
    for (xoff = [-battery_d/2 - 2, battery_d/2 + 2])
        translate([battery_x + xoff, battery_y, wall + 4])
            cube([rail_w, rail_h, rail_d], center=true);

    // Two top retaining clips with a center thumb notch.
    for (xoff = [-7, 7])
        translate([battery_x + xoff, battery_y + battery_l/2 + 1.5, wall + 5])
            cube([9, 3, 9], center=true);

    // Rear support ribs under battery.
    for (yoff = [-22, 0, 22])
        translate([battery_x, battery_y + yoff, wall + 1.5])
            cube([battery_d + 5, 2.2, 3], center=true);
}

module inside_label() {
    translate([-case_w/2 + 15, 18, wall + 0.2])
        linear_extrude(height = label_depth)
            text(str(device_serial), size = label_size, halign="center", valign="center");
}

module back_shell() {
    difference() {
        union() {
            shell_body(back_shell_d);
            heltec_clip_standoffs();
            screw_bosses(back_shell_d, false);
            battery_clip_holder();
            inside_label();
        }

        side_button_cutouts();
    }
}
// ---------- Preview ----------
translate([-case_w/2 - preview_gap/2, 0, 0])
    front_shell();

translate([case_w/2 + preview_gap/2, 0, 0])
    back_shell();

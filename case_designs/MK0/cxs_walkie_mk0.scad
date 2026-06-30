include <cxs_walkie_params.scad>

$fn = 64;

// ---------- Helpers ----------
module rounded_rect_2d(w, h, r) {
    hull() {
        translate([ w/2-r,  h/2-r]) circle(r=r);
        translate([-w/2+r,  h/2-r]) circle(r=r);
        translate([ w/2-r, -h/2+r]) circle(r=r);
        translate([-w/2+r, -h/2+r]) circle(r=r);
    }
}

module rounded_box(w, h, d, r) {
    linear_extrude(height=d)
        rounded_rect_2d(w, h, r);
}

module slot_cutout(w, h, d=4) {
    translate([0, 0, -0.1])
        linear_extrude(height=d)
            rounded_rect_2d(w, h, min(w,h)/2);
}

module front_outer() {
    translate([0, case_h/2, 0])
        rounded_box(case_w, case_h, front_shell_d, corner_r);
}

module front_inner_void() {
    translate([0, case_h/2, wall])
        rounded_box(case_w - 2*wall, case_h - 2*wall, front_shell_d + 1, max(1, corner_r - wall));
}

module front_cutouts() {
    // Speaker grille - vertical old-school slots
    for (i = [0 : speaker_grille_slots-1]) {
        x = (i - (speaker_grille_slots-1)/2) * speaker_grille_slot_gap;
        translate([x, speaker_y, -0.2])
            slot_cutout(speaker_grille_slot_w, speaker_grille_slot_h, wall + 1);
    }

    // OLED window
    translate([0, oled_y, -0.2])
        linear_extrude(height=wall + 1)
            rounded_rect_2d(oled_window_w, oled_window_h, 2);

    // Microphone sound hole
    translate([0, mic_y, -0.2])
        cylinder(d=mic_sound_hole_d, h=wall + 1);

    // Left-side PTT hole through side wall
    translate([left_side_x - 0.2, ptt_y, front_shell_d/2])
        rotate([0,90,0]) cylinder(d=ptt_hole_d, h=wall + 2);

    // Right-side channel hole
    translate([right_side_x - wall - 0.2, ptt_y + 12, front_shell_d/2])
        rotate([0,90,0]) cylinder(d=channel_hole_d, h=wall + 2);

    // Right-side power toggle hole below channel
    translate([right_side_x - wall - 0.2, ptt_y - 12, front_shell_d/2])
        rotate([0,90,0]) cylinder(d=power_hole_d, h=wall + 2);
}

module screw_boss(x, y) {
    translate([x, y, wall])
        difference() {
            cylinder(d=insert_d + 3.2, h=front_shell_d - wall - 1);
            translate([0,0,-0.1]) cylinder(d=insert_d, h=insert_h + 0.2);
        }
}

module all_screw_bosses() {
    screw_boss(-case_w/2 + corner_screw_margin_x, corner_screw_margin_y);
    screw_boss( case_w/2 - corner_screw_margin_x, corner_screw_margin_y);
    screw_boss(-case_w/2 + corner_screw_margin_x, case_h - corner_screw_margin_y);
    screw_boss( case_w/2 - corner_screw_margin_x, case_h - corner_screw_margin_y);
}

module heltec_cradle() {
    // Simple upper front ledge/rails for fit checking
    translate([0, heltec_y, wall + 1]) {
        // lower shelf
        translate([0, -heltec_pocket_l/2, 0])
            cube([heltec_pocket_w + 4, 2.2, 3], center=true);
        // side rails
        translate([-heltec_pocket_w/2 - 1.4, 0, 1.5])
            cube([2, heltec_pocket_l, 3], center=true);
        translate([ heltec_pocket_w/2 + 1.4, 0, 1.5])
            cube([2, heltec_pocket_l, 3], center=true);
        // top stop
        translate([0, heltec_pocket_l/2, 0])
            cube([heltec_pocket_w + 4, 2.2, 3], center=true);
    }
}

module battery_cradle() {
    // Half-round vertical cradle in back half area, visible in front shell for layout check
    translate([0, battery_y, front_shell_d - battery_d/2 - 2])
        rotate([90,0,0])
            difference() {
                cylinder(d=battery_d + 2*battery_cradle_wall, h=battery_l, center=true);
                cylinder(d=battery_d, h=battery_l + 1, center=true);
                translate([-battery_d, -battery_d, -battery_l/2 - 1])
                    cube([2*battery_d, battery_d, battery_l + 2]);
            }
}

module speaker_pocket() {
    translate([0, speaker_y, wall + 0.8])
        difference() {
            cube([speaker_w + 3, speaker_l + 3, 3], center=true);
            translate([0,0,-0.1]) cube([speaker_w, speaker_l, 3.4], center=true);
        }
}

module mic_shelf() {
    translate([0, mic_y - 5, wall + 2])
        cube([mic_w + 4, 2.4, 4], center=true);
}

module amp_shelf() {
    translate([18, amp_y, wall + 2])
        cube([amp_w + 4, amp_l + 4, 2.4], center=true);
}

module inside_label() {
    translate([0, 18, front_shell_d - label_depth])
        linear_extrude(height=label_depth + 0.05)
            text(str(device_serial, " ", case_revision), size=label_size, halign="center", valign="center");
}

// ---------- Assemblies ----------
module front_shell_mk0() {
    difference() {
        front_outer();
        front_inner_void();
        front_cutouts();
    }
    all_screw_bosses();
    heltec_cradle();
    speaker_pocket();
    mic_shelf();
    amp_shelf();
    battery_cradle();
    inside_label();
}

module back_shell_mk0() {
    // Simple placeholder back shell for later refinement
    translate([case_w + 12, 0, 0])
        difference() {
            translate([0, case_h/2, 0]) rounded_box(case_w, case_h, back_shell_d, corner_r);
            translate([0, case_h/2, wall]) rounded_box(case_w - 2*wall, case_h - 2*wall, back_shell_d + 1, max(1, corner_r - wall));
            // Screw through holes
            for (x=[-case_w/2 + corner_screw_margin_x, case_w/2 - corner_screw_margin_x])
                for (y=[corner_screw_margin_y, case_h - corner_screw_margin_y])
                    translate([x,y,-0.1]) cylinder(d=screw_clearance_d, h=back_shell_d + 1);
        }
}

front_shell_mk0();
back_shell_mk0();

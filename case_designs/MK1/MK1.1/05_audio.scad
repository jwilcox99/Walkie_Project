// CXS Walkie MK1 - Audio Features
include <cxs_params.scad>

module speaker_grille_cutouts() {
    total_w = speaker_grille_slots * speaker_grille_slot_w
            + (speaker_grille_slots - 1) * speaker_grille_slot_gap;

    for (i = [0:speaker_grille_slots-1]) {
        x = speaker_x - total_w/2 + i*(speaker_grille_slot_w + speaker_grille_slot_gap);
        translate([x, speaker_y, -1])
            cube([speaker_grille_slot_w, speaker_grille_slot_h, front_shell_d + 2], center=true);
    }
}

module mic_standoffs() {

    // One-piece microphone mount
    bridge_w = mic_mount_hole_spacing + mic_standoff_d;
    bridge_d = mic_standoff_d;
    bridge_h = mic_standoff_h;

    difference() {

        union() {

            // Main bridge
            translate([
                mic_x + mic_mount_x_offset - bridge_w/2,
                mic_y + mic_mount_y_offset - bridge_d/2,
                wall
            ])
                cube([bridge_w, bridge_d, bridge_h]);

            // Heat-set bosses
            for (xoff = [-mic_mount_hole_spacing/2,
                          mic_mount_hole_spacing/2]) {

                translate([
                    mic_x + mic_mount_x_offset + xoff,
                    mic_y + mic_mount_y_offset,
                    wall
                ])
                    cylinder(
                        d = mic_standoff_d,
                        h = mic_standoff_h
                    );
            }
        }

        // Screw / insert holes
        for (xoff = [-mic_mount_hole_spacing/2,
                      mic_mount_hole_spacing/2]) {

            translate([
                mic_x + mic_mount_x_offset + xoff,
                mic_y + mic_mount_y_offset,
                wall - 0.1
            ])
                cylinder(
                    d = mic_screw_d,
                    h = mic_standoff_h + 0.2
                );
        }
    }
}

module mic_hole_pattern() {
    // Centered 2-row microphone grille.
    for (x = [-mic_hole_spacing_x, 0, mic_hole_spacing_x])
        for (y = [-mic_hole_spacing_y/2, mic_hole_spacing_y/2])
            translate([mic_x + x, mic_y + y, -1])
                cylinder(d = mic_hole_d, h = front_shell_d + 2);
}

// No amp shelf in MK1. A separate amp standoff can be printed later after final placement.

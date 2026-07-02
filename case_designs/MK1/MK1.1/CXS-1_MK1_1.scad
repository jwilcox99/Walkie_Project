// CXS Walkie MK1.1 Assembly
// Main file to open/render.

include <cxs_params.scad>

include <01_shell.scad>
include <02_heltec_mount.scad>
include <03_battery_holder.scad>
include <04_controls.scad>
include <05_audio.scad>
$fn = 48;

module front_shell() {
    difference() {
        union() {
            shell_body(front_shell_d);
            screw_bosses(front_shell_d, true);
            mic_standoffs();
        }

        translate([oled_x, oled_y, -1])
            cube([oled_window_w, oled_window_h, front_shell_d + 2], center=true);
        
        speaker_grille_cutouts();
        mic_hole_pattern();
    }
}

module back_shell() {
    difference() {
        union() {
            shell_body(back_shell_d);
            screw_bosses(back_shell_d, false);
            battery_clip_holder();
            heltec_clip_standoffs();
        }

        side_cutouts();
    }
}

// ---------- Preview ----------
color("cyan")
translate([-50, 0, 0])
    front_shell();

color("orange")
translate([50, 0, 0])
    back_shell();

// CXS Walkie Enclosure Parameter Library
// Units: millimeters

// ---------- Identity ----------
device_serial = "CXS-1";
case_revision = "MK1";

// ---------- Case ----------
case_h = 145;
case_w = 65;
case_d = 38;

wall = 2.4;
corner_r = 11;

// Mk0 split: shallow faceplate + deeper electronics tub
front_shell_d = 10;
back_shell_d = case_d - front_shell_d;

// ---------- Clearances ----------
fit_clearance = 0.6;
loose_clearance = 1.0;
button_clearance = 0.4;
hole_depth = 30;

// ---------- Heltec V4 ----------
heltec_w = 25.3;
heltec_l = 52.5;
heltec_h = 10.7;

// ---------- Antenna ----------
antenna_hole_d = 6.5;
antenna_y = heltec_y + 18;
antenna_x = right_side_x - 9;

// ---------- Heltec Raised Clip Mount ----------
heltec_standoff_h = 20;        // raise/lower board here
heltec_post_d = 5.0;
heltec_clip_w = 4.0;
heltec_clip_lip_h = 1.6;
heltec_clip_lip_overhang = 1.2;
heltec_clip_clearance = 0.8;

usb_c_w = 9.0;
usb_c_h = 3.5;

oled_window_w = 33.5;
oled_window_h = 19.5;

// ---------- Speaker ----------
speaker_w = 15.6;
speaker_l = 24.5;
speaker_h = 4.2;

speaker_grille_slot_w = 1.6;
speaker_grille_slot_h = 20;
speaker_grille_slot_gap = 2.6;
speaker_grille_slots = 6;

// ---------- Battery ----------
battery_d = 20.0;
battery_l = 70.0;
battery_cradle_wall = 1.8;

// ---------- Buttons ----------
ptt_hole_d = 24.4;
ptt_depth = 28;
channel_hole_d = 15.5;
channel_depth = 17;
power_hole_d = 5.7;
power_depth = 16;

// ---------- Mic ----------
mic_hole_d = 1.5;
mic_hole_spacing_x = 4;
mic_hole_spacing_y = 4;
mic_standoff_d = 4.2;
mic_standoff_h = 5.0;
mic_screw_d = 2.0;
mic_mount_hole_spacing = 10;
mic_mount_y_offset = 6;

// ---------- Amp ----------
amp_w = 18.5;
amp_l = 20.0;
amp_terminal_h = 11;

// ---------- Control Positions ----------
ptt_y = 70;

channel_y = 75;
power_y = 55;

// ---------- Layout Positions ----------
// Origin: X centered, Y bottom = 0, Z front face = 0

speaker_y = 126;
oled_y = 104;
ptt_y = 70;
mic_y = 32; 

battery_y = 40;
heltec_y = 104;
amp_y = 36;

// X placement
center_x = 0;
left_side_x = -case_w / 2;
right_side_x = case_w / 2;

// Internal offsets
// OLED remains visually centered; Heltec shifts toward USB side.
oled_x = 0;
heltec_x = 0;
battery_x = 8;
amp_x = -18;
speaker_x = 0;
mic_x = 0;

// ---------- Fasteners ----------
insert_d = 4.2;
insert_h = 5.0;
screw_clearance_d = 3.2;

corner_screw_margin_x = 8;
corner_screw_margin_y = 10;

// ---------- Text / Branding ----------
label_depth = 0.5;
label_size = 5;

// ---------- Orientation ----------
// Heltec is mounted horizontally so OLED aligns with landscape window.
heltec_rot = 90;

usb_cutout_w = 14;
usb_cutout_h = 9;
usb_y_offset = 8;
usb_z_offset = 6;